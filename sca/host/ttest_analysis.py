import scared
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
import estraces
import time
import threading


class TTestAnalysis:
    def __init__(self, proj_name=None, offset=0, label="", N=100000, prng_off=False, init_ths=True):
        self.ttest = None
        self.proj_name = proj_name
        self.offset = offset
        self.label = label
        self.N = N
        self.prng_off = prng_off
        self.num_traces_0 = 0
        self.num_traces_1 = 0
        self.filename = f"{self._res_filename()}"
        self.ths_0 = None
        self.ths_1 = None
        self.mean_trace_0 = None
        self.mean_trace_1 = None
        if init_ths:
            self.read_ths()

    def ths_name(self, random, prng_off=False, N=1000):
        return f"traces/{self.proj_name}_N{N}_prngoff{int(prng_off)}_r{random}_o{self.offset}_{self.label}.ets"

    def read_ths(self, verbose=True):
        if self.ths_0 is not None:
            del self.ths_0
        if self.ths_1 is not None:
            del self.ths_1
        _ths_filename0 = self.ths_name(random=0, prng_off=self.prng_off, N=self.N)
        self.ths_0 = estraces.read_ths_from_ets_file(_ths_filename0)
        _ths_filename1 = self.ths_name(random=1, prng_off=self.prng_off, N=self.N)
        self.ths_1 = estraces.read_ths_from_ets_file(_ths_filename1)
        if verbose:
            print(self.ths_0)
            print(self.ths_1)

    def _res_filename(self):
        return f"{self.proj_name}_N{self.N}_prngoff{int(self.prng_off)}_o{self.offset}_{self.label}"

    def run_ttest(self, num_traces=None, chunk=5000, append=True, force_equal=False, verbose=False, loop_verbose=True):
        if num_traces is None:
            num_traces = min(len(self.ths_0.samples), len(self.ths_1.samples)) # if force_equal else max(len(self.ths_0.samples), len(self.ths_1.samples))
        self.ttest = scared.ttest.TTestAnalysis() if not append or self.ttest is None else self.ttest
        start = 0 if not append else min(self.num_traces_0, self.num_traces_1)
        r = range(start, num_traces, chunk) if not loop_verbose else tqdm(range(start, num_traces, chunk), desc='Running T-Test')
        for i in r:
            if i == start: ## first
                start_0 = self.num_traces_0
                start_1 = self.num_traces_1
            else:
                start_0 = i
                start_1 = i
            if i + chunk >= num_traces: ## last
                end_0 = len(self.ths_0.samples) if not force_equal else num_traces
                end_1 = len(self.ths_1.samples) if not force_equal else num_traces
            else:
                end_0 = i + chunk
                end_1 = i + chunk
            if verbose:
                print(f"Processing traces {start_0} to {end_0} (Class 0) and {start_1} to {end_1} (Class 1)")
            ths_0 = estraces.read_ths_from_ram(self.ths_0.samples[start_0:end_0])
            ths_1 = estraces.read_ths_from_ram(self.ths_1.samples[start_1:end_1])
            container = scared.TTestContainer(ths_0, ths_1)
            self.ttest.run(container)
        self.num_traces_0 = num_traces if force_equal else len(self.ths_0.samples)
        self.num_traces_1 = num_traces if force_equal else len(self.ths_1.samples)

    def plot_means(self, plot0=True, plot1=True, put_title=True, put_legend=True, num_traces=None, size=None):
        if self.mean_trace_0 is None or self.mean_trace_1 is None:
            if num_traces is None or num_traces > len(self.ths_0.samples):
                num_traces = len(self.ths_0.samples)
            self.compute_means(num_traces=num_traces)
        fig, ax = plt.subplots(figsize=size) if size is not None else plt.subplots()
        if plot0:
             ax.plot(self.mean_trace_0, label='Class 0 Mean')
        if plot1:
             ax.plot(self.mean_trace_1, label='Class 1 Mean')
        if put_legend:
            fig.legend()
        if put_title:
            plt.title('Mean Traces')
        return ax

    def compute_means(self, num_traces=None):
        if num_traces is None or num_traces > len(self.ths_0.samples):
            num_traces = len(self.ths_0.samples)
        step = len(self.ths_0.samples) // num_traces
        if len(self.ths_0.samples) % num_traces != 0:
            step += 1
        self.mean_trace_0 = np.mean(self.ths_0.samples[::step], axis=0)
        self.mean_trace_1 = np.mean(self.ths_1.samples[::step], axis=0)

    def plot_traces(self, num_traces=5, put_title=True):
        fig, ax = plt.subplots()
        for i in range(10):
            ax.plot(self.ths_0.samples[i], color='blue', alpha=0.3)
            ax.plot(self.ths_1.samples[i], color='orange', alpha=0.3)
        if put_title:
            plt.title(f'Sample Traces (First {num_traces} from each class)')
        plt.show()        

    def plot_ttest(self, put_title=True, ylimit=None, ax=None, color='black', plt_show=True, hline=True, label_fontsize=None, put_label=True, hline_val=4.5, size=None):
        if ax is None:
            fig, ax = plt.subplots(figsize=size) if size is not None else plt.subplots()
        ax.plot(np.abs(self.ttest.result), color=color)
        if ylimit is not None:
            ax.set_ylim(0, ylimit)
        if hline:
            ax.axhline(y=hline_val, color='red', linestyle='--', alpha=0.7)

        if put_label:
            if label_fontsize is not None:
                plt.xlabel("sample index", fontsize=label_fontsize)
                plt.ylabel(r"$t\text{-value}$", fontsize=label_fontsize)
            else:
                plt.xlabel("sample index")
                plt.ylabel(r"$t\text{-value}$")
        if put_title:
            plt.title(f'T-Test Results ({self.num_traces_0}-{self.num_traces_1} traces)')
        if plt_show:
            plt.show()
        return ax

    def save_ttest_results(self):
        np.save('ttest_results/'+ self.filename + '_ttest.npy', self.ttest.result)
        np.save('ttest_results/'+ self.filename + '_mean0.npy', self.mean_trace_0)
        np.save('ttest_results/'+ self.filename + '_mean1.npy', self.mean_trace_1)

    def load_ttest_results(self, load_means=True):
        self.ttest = scared.ttest.TTestAnalysis()
        self.ttest.result = np.load('ttest_results/'+ self.filename + '_ttest.npy')
        if load_means:
            self.mean_trace_0 = np.load('ttest_results/'+ self.filename + '_mean0.npy')
            self.mean_trace_1 = np.load('ttest_results/'+ self.filename + '_mean1.npy')

    def live_update_ttest(self, refresh_interval=5, chunk=5000, force_equal=True, ylimit=None, hline_val=4.5, color='black'):

        # ---- Create figure once ----
        fig, ax = plt.subplots()
        line, = ax.plot([], [], color=color)
        ax.axhline(y=hline_val, linestyle='--', color='red', alpha=0.7)

        ax.set_xlabel("sample index")
        ax.set_ylabel("t-value")

        self._stop_live = False
        self._last_processed = 0
        self.loop_verbose = True

        # ---- Worker thread ----
        def worker():
            while not self._stop_live:

                self.read_ths(verbose=False)

                available = min(len(self.ths_0.samples),
                                len(self.ths_1.samples))

                if available > self._last_processed:

                    self.run_ttest(num_traces=available, chunk=chunk, append=True, force_equal=force_equal,
                                   loop_verbose=self.loop_verbose)
                    self.loop_verbose = False

                    self._last_processed = available

                    tvals = np.abs(self.ttest.result)

                    # Update plot SAFELY
                    line.set_data(np.arange(len(tvals)), tvals)
                    ax.set_xlim(0, len(tvals))

                    if ylimit is None:
                        ax.set_ylim(0, max(5, np.max(tvals) * 1.1))
                    else:
                        ax.set_ylim(0, ylimit)

                    ax.set_title(
                        f"T-Test ({self.num_traces_0}-{self.num_traces_1} traces)"
                    )

                    fig.canvas.draw_idle()

                time.sleep(refresh_interval)

        # Start background thread
        self._thread = threading.Thread(target=worker, daemon=True)
        self._thread.start()

        print("Live monitor started. Call stop_live_update() to stop.")
