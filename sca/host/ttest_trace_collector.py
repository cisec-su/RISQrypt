import random
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
    def __init__(self, proj_name, label="", input_len=32, output_len=16, offset=0):
        self.proj_name = proj_name
        self.label = label
        self.input_len = input_len
        self.output_len = output_len
        self.bootloader = "python3 ../../sdk/toolchain/bootloader.py"
        self.bs_file = "../vivado/risqrypt_cw305.runs/impl_1/fpga_top.bit"
        self.offset = offset
        self.scope = None
        self.ab_seq_seed = None
        self.ab_seq_chunk = 1
        self.ab_seq_chunk1 = 1000
        self.const_seed = os.urandom(self.input_len//2)

    def set_offset(self, offset):
        self.offset = offset
        if self.scope is not None:
            self.scope.adc.offset = offset

    def fw_dir(self):
        return f"../fw/victims/{self.proj_name}"

    def bin_file(self):
        return f"{self.fw_dir()}/{self.proj_name}"

    def init_target(self, force=True):
        self.scope = cw.scope()
        self.target = cw.target(self.scope, CW305_rq, force=force, fpga_id='100t', platform='cw305', bsfile=self.bs_file)
        self.target.output_len = self.output_len

    def set_prng_on(self):
        self.target.simpleserial_write('l', os.urandom(8))
        self.target.simpleserial_wait_ack()
        time.sleep(0.5)

    def set_prng_off(self):
        self.target.simpleserial_write('g', [])
        self.target.simpleserial_wait_ack()

    def ths_name(self, random, prng_off=False, N=1000):
        return f"traces/{self.proj_name}_N{N}_prngoff{int(prng_off)}_r{random}_o{self.offset}_{self.label}.ets"

    def delete_traces(self, N=5000, prng_off=False):
        filename_0 = self.ths_name(random=0, prng_off=prng_off, N=N)
        if os.path.exists(filename_0):
            os.remove(filename_0)
        filename_1 = self.ths_name(random=1, prng_off=prng_off, N=N)
        if os.path.exists(filename_1):
            os.remove(filename_1)

    def get_analysis_obj(self, N=5000, prng_off=False):
        return TTestAnalysis(self.proj_name, offset=self.offset, label=self.label, N=N, prng_off=prng_off)

    def _default_setup(self, freq, gain, samples, mul=4):
        self.scope.adc.offset = self.offset
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
        return self.scope.adc_test() == 'pass'

    def default_setup(self, freq, gain, samples, mul=4, retry=True):
        setup_succ = self._default_setup(freq, gain, samples, mul)
        if not setup_succ:
            if retry:
                self.scope.reset_fpga()
                assert self._default_setup(freq, gain, samples, mul), "ADC test failed!"
            else:
                assert False, "ADC test failed!"

    def build_fw(self, verbose=False, **make_flags):
        cmd = "make"

        if verbose:
            cmd += " VERBOSE=1"

        for key, value in make_flags.items():
            if isinstance(value, bool):
                if value:
                    cmd += f" {key}=1"
            else:
                cmd += f" {key}={value}"

        subprocess.run(
            cmd,
            cwd=self.fw_dir(),
            shell=True,
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

    def set_input(self, seed, prng_off=False):
        seed_mask = os.urandom(self.input_len//2) if not prng_off else bytes([0]*(self.input_len//2))
        seed_temp = bytes([(seed[i] ^ seed_mask[i]) for i in range(self.input_len//2)])
        seed_full = seed_temp + seed_mask
        return seed_full

    def generate_balanced_AB_chunk1(self, N):
        if N % self.ab_seq_chunk1 != 0:
            raise ValueError("self.ab_seq_chunk1 must divide N")
        if self.ab_seq_chunk1 % 2 != 0:
            raise ValueError("self.ab_seq_chunk1 must be even for balance")

        rng = np.random.default_rng(self.ab_seq_seed) if self.ab_seq_seed is not None else np.random.default_rng(random.randint(0, 2**32 - 1))

        seq = np.empty((N, 2), dtype=int)

        num_blocks = N // self.ab_seq_chunk1

        for b in range(num_blocks):
            pair_types = np.array([0] * (self.ab_seq_chunk1 // 2) + [1] * (self.ab_seq_chunk1 // 2))
            rng.shuffle(pair_types)

            for i, t in enumerate(pair_types):
                idx = b * self.ab_seq_chunk1 + i
                if t == 0:
                    seq[idx] = (0, 1)
                else:
                    seq[idx] = (1, 0)

        return seq.ravel()

    def generate_balanced_AB(self, N):
        seq = np.empty(N*2, dtype=int)

        rng = np.random.default_rng(self.ab_seq_seed) if self.ab_seq_seed is not None else np.random.default_rng(random.randint(0, 2**32 - 1))

        idx = 0

        while idx < N:
            n = min(self.ab_seq_chunk, N - idx)

            batch = np.array([0] * n + [1] * n)
            rng.shuffle(batch)

            seq[2*idx:2*idx + 2 * n] = batch
            idx += n

        return seq

    def collect_traces(self, N=5000, prng_off=False, overwrite=False, check_output=True, init_input=False, dummy_inbetween_0=False, dummy_inbetween_1=False, rand2rand=False, random_order=True, balanced_chunk1=False, coin_flip=False):

        if random_order and coin_flip:
            raise ValueError("random_order and coin_flip cannot both be True")

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
        const_seed = self.const_seed

        if random_order:
            ab_seq = self.generate_balanced_AB(N//2) if self.ab_seq_chunk > 1 or not balanced_chunk1 else self.generate_balanced_AB_chunk1(N//2)

        for i in tnrange(N, desc='Capturing traces'):
            if coin_flip:
                if random.random() < 0.5:
                    trace_class = 0
                else:
                    trace_class = 1
            elif random_order:
                trace_class = ab_seq[i]
            else:
                trace_class = i % 2

            if trace_class == 0:
                es_writer = es_writer_0
            else:
                es_writer = es_writer_1

            if trace_class == 0 and not rand2rand:
                seed = const_seed
            else:
                seed = os.urandom(self.input_len//2)

            seed_full = self.set_input(seed, prng_off)
            ret = cw.capture_trace(self.scope, self.target, seed_full, None)
            if not ret:
                print("Failed capture")
                continue
            if check_output:
                assert self.check_output(ret.textout, seed), "Output mismatch!"
            es_writer.write_samples(np.array(ret.wave))
            es_writer.write_metadata('s', np.frombuffer(seed, dtype=np.uint8))

            if (trace_class == 0 and dummy_inbetween_0) or (trace_class == 1 and dummy_inbetween_1):
                dummy_seed = self.set_input(bytes([0]*(self.input_len//2)), True)
                ret = cw.capture_trace(self.scope, self.target, dummy_seed, None)
                if not ret:
                    print("Failed capture")
                    continue

        es_writer_0.close()
        es_writer_1.close()