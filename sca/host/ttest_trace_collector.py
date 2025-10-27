import estraces
from tqdm.notebook import tnrange
import numpy as np
import os
import chipwhisperer as cw
import time
from .CW305_rq import CW305_rq
from .ttest_analysis import TTestAnalysis
import subprocess


class TTestTraceCollector:
    def __init__(self, proj_name, label=None, input_len=32, output_len=16):
        self.proj_name = proj_name
        self.label = label
        self.input_len = input_len
        self.output_len = output_len
        self.bootloader = "python3 ../../sdk/toolchain/bootloader.py"
        self.bs_file = "../vivado/risqrypt_cw305.runs/impl_1/fpga_top.bit"

    def fw_dir(self):
        return f"../fw/victims/{self.proj_name}"

    def bin_file(self):
        return f"{self.fw_dir()}/{self.proj_name}"

    def init_target(self, force=True):
        self.scope = cw.scope()
        self.target = cw.target(self.scope, CW305_rq, force=force, fpga_id='100t', platform='cw305', bsfile=self.bs_file)
        self.target.output_len = self.output_len

    def set_prng_on(self):
        self.target.simpleserial_write('l', [])
        self.target.simpleserial_wait_ack()

    def set_prng_off(self):
        self.target.simpleserial_write('g', [])
        self.target.simpleserial_wait_ack()

    def ths_name(self, random, prng_off=False, N=1000):
        if self.label is not None:
            return f"traces/{self.proj_name}_N{N}_prngoff{int(prng_off)}_r{random}_{self.label}"
        else:
            return f"traces/{self.proj_name}_N{N}_prngoff{int(prng_off)}_r{random}"

    def read_ths(self, N=5000, prng_off=False):
        filename_0 = self.ths_name(random=0, prng_off=prng_off, N=N)
        ths_0 = estraces.read_ths_from_ets_file(filename_0)
        filename_1 = self.ths_name(random=1, prng_off=prng_off, N=N)
        ths_1 = estraces.read_ths_from_ets_file(filename_1)
        print(ths_0)
        print(ths_1)
        return ths_0, ths_1

    def get_analysis_obj(self, N=5000, prng_off=False):
        ths_0, ths_1 = self.read_ths(N=N, prng_off=prng_off)
        return TTestAnalysis(ths_0, ths_1)

    def default_setup(self, freq, gain, samples, mul=4):
        self.scope.adc.offset = 0
        self.scope.adc.basic_mode = "rising_edge"
        self.scope.trigger.triggers = "tio4"
        self.scope.io.tio1 = "serial_rx"
        self.scope.io.tio2 = "serial_tx"
        self.scope.io.hs2 = "disabled"
        #
        self.target.vccint_set(1.0)
        # we only need PLL1:
        self.target.pll.pll_enable_set(True)
        self.target.pll.pll_outenable_set(False, 0)
        self.target.pll.pll_outenable_set(True, 1)
        self.target.pll.pll_outenable_set(False, 2)
        self.target.pll.pll_outfreq_set(freq, 1)
        # 1ms is plenty of idling time
        self.target.clkusbautooff = True
        self.target.clksleeptime = 1
        # dedicated to husky
        # self.scope.clock.clkgen_freq = freq
        self.scope.clock.clkgen_src = 'extclk'
        self.scope.clock.adc_mul = mul
        self.scope.adc.stream_mode = True if samples > 120000 else False
        for _ in range(5):
            self.scope.clock.reset_adc()
            time.sleep(1)
            if self.scope.clock.adc_locked:
                break
        assert (self.scope.clock.adc_locked), "ADC failed to lock"
        self.scope.adc.samples = samples
        self.scope.gain.db = gain
        if freq > 10E6:
            assert self.scope.adc_test() == 'pass', "ADC test failed!"

    def build_fw(self):
        subprocess.run(
            ["make"],
            cwd=self.fw_dir(),
            check=True
        )

    def program_fw(self):
        cmd = f"{self.bootloader} -f {self.bin_file()} -q"
        subprocess.run(
            cmd,
            shell=True,
            check=True
        )

    def init_input(self):
        raise NotImplementedError("Please implement init_input to set up input conditions")

    def check_output(self, func):
        raise NotImplementedError("Please implement check_output to use output checking")

    def collect_traces(self, N=5000, prng_off=False, overwrite=False, check_output=True, init_input=False):
        if prng_off:
            self.set_prng_off()
        else:
            self.set_prng_on()
        if init_input:
            self.init_input()
        filename_0 = self.ths_name(random=0, prng_off=prng_off, N=N)
        es_writer_0 = estraces.ETSWriter(filename=filename_0, overwrite=overwrite)
        filename_1 = self.ths_name(random=1, prng_off=prng_off, N=N)
        es_writer_1 = estraces.ETSWriter(filename=filename_1, overwrite=overwrite)
        const_seed = os.urandom(self.input_len//2)
        for _ in tnrange(N, desc='Capturing traces'):
            # const input
            const_seed_mask = os.urandom(self.input_len//2) if not prng_off else bytes([0]*(self.input_len//2))
            const_seed_temp = bytes([(const_seed[i] ^ const_seed_mask[i]) for i in range(self.input_len//2)])
            const_seed_full = const_seed_temp + const_seed_mask
            ret = cw.capture_trace(self.scope, self.target, const_seed_full, None)
            if not ret:
                print("Failed capture")
                continue
            if check_output:
                assert self.check_output(ret.textout, const_seed), "Output mismatch!"
            es_writer_0.write_samples(np.array(ret.wave))
            es_writer_0.write_metadata('s', np.frombuffer(const_seed))
            # rand input
            seed = os.urandom(self.input_len//2)
            seed_mask = os.urandom(self.input_len//2) if not prng_off else bytes([0]*(self.input_len//2))
            seed_full = seed + seed_mask
            ret = cw.capture_trace(self.scope, self.target, seed_full, None)
            if not ret:
                print("Failed capture")
                continue
            if check_output:
                assert self.check_output(ret.textout, seed), "Output mismatch!"
            es_writer_1.write_samples(np.array(ret.wave))
            es_writer_1.write_metadata('s', np.frombuffer(seed))
        es_writer_0.close()
        es_writer_1.close()