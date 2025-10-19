from time import time
from chipwhisperer.capture.targets._base import TargetTemplate
from chipwhisperer.hardware.naeusb.naeusb import packuint32

from chipwhisperer.capture.targets.CW305 import CW305
from chipwhisperer.capture.targets.SimpleSerial import SimpleSerial


from chipwhisperer.logging import *


class _DummySer():
    def flush(self):
        pass


class CW305_rq(CW305, SimpleSerial):

    """CW305 Target for RISQrypt Firmware.
       Communicates over SimpleSerial protocol over CW305 USB Interface.
    """
    def __init__(self):
        TargetTemplate.__init__(self)

        #NOTE: default values to make linter happy - these are never correct
        self.REG_DATA = 0x0
        self.REG_DONE = 0x1

        self._naeusb = None
        self.pll = None
        self.fpga = None
        self.ss2 = None
        self.platform = None

        self.ser = _DummySer()

        self.hw = None
        self.oa = None

        self._woffset_sam3U = 0x000
        self.registers = 2 # number of registers we expect to find
        self.bytecount_size = 7 # pBYTECNT_SIZE in Verilog

        self._clksleeptime = 1
        self._clkusbautooff = True
        self.check_done = False
        self.last_key = bytearray([0]*16)
        self.target_name = 'CW305_RISQrypt'

        self.done_seen = False
        self.wait_done_sleep = 0.0001

    def slurp_defines(self, defines_files=None):
        raise NotImplementedError(f"SLURP is not implemented for {self.target_name} target")

    def _reg_not_exist_str(reg_name):
        return reg_name + f" Register doesn't exist in {self.target_name} target"

    @property
    def crypt_type(self):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_TYPE"))

    @property
    def crypt_rev(self):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_REV"))        

    @property
    def core_type(self):
        raise NotImplementedError(self._reg_not_exist_str("CORE_TYPE"))    

    def loadEncryptionKey(self, key):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_KEY")) 

    def loadInput(self, inputtext):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_TEXTIN"))

    def is_done(self):
        """Check if FPGA is done"""
        if self.check_done:
            if not self.done_seen:
                status = self.fpga_read(1, 1)
                if status[0] == 0xff:
                    self.done_seen = True
                else:
                    return False
            return True
        else:
            return True

    def readOutput(self):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_TEXTOUT"))

    def go(self):
        raise NotImplementedError(self._reg_not_exist_str("CRYPT_GO"))

    def _con(self, *args, slurp=False, **kwargs):
        CW305._con(self, *args, slurp=slurp, **kwargs)

    def write(self, data, timeout=0):
        self.fpga_write(self.REG_DATA, data)

    def read(self, num_char = 0, timeout=250):
        return self.fpga_read(self.REG_DATA, num_char).decode()

    def _wait_done_with_timeout(self, timeout=250):
        ctr = 0
        while not self.is_done():
            time.sleep(self.wait_done_sleep)
            ctr += 1
            if ctr > timeout * (1/self.wait_done_sleep):
                raise TimeoutError("Timeout waiting for DONE signal from target")

    def simpleserial_wait_ack(self, timeout=500):
        self._wait_done_with_timeout(timeout=timeout)
        return SimpleSerial.simpleserial_wait_ack(self, timeout=timeout)

    def simpleserial_write(self, cmd, data, end='\n'):
        self.done_seen = False
        SimpleSerial.simpleserial_write(self, cmd, data, end=end)

    def simpleserial_read(self, cmd, pay_len, end='\n', timeout=250, ack=True):
        self._wait_done_with_timeout(timeout=timeout)
        return SimpleSerial.simpleserial_read(self, cmd, pay_len, end=end, ack=ack)