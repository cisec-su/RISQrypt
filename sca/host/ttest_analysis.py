import scared
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
import estraces


class TTestAnalysis:
    def __init__(self, proj_name=None, offset=0, label="", N=100000, prng_off=False, init_ths=True):
        self.ttest = None
        self.proj_name = proj_name
        self.offset = offset
        self.label = label
        self.N = N
        self.prng_off = prng_off
        self.num_traces = 0
        self.filename = f"{self._res_filename()}"
        self.ths_0 = None
        self.ths_1 = None
        self.mean_trace_0 = None
        self.mean_trace_1 = None
        if init_ths:
            self.read_ths()

    def ths_name(self, random, prng_off=False, N=1000):
        return f"traces/{self.proj_name}_N{N}_prngoff{int(prng_off)}_r{random}_o{self.offset}_{self.label}.ets"

    def read_ths(self):
        if self.ths_0 is not None:
            del self.ths_0
        if self.ths_1 is not None:
            del self.ths_1
        _ths_filename0 = self.ths_name(random=0, prng_off=self.prng_off, N=self.N)
        self.ths_0 = estraces.read_ths_from_ets_file(_ths_filename0)
        _ths_filename1 = self.ths_name(random=1, prng_off=self.prng_off, N=self.N)
        self.ths_1 = estraces.read_ths_from_ets_file(_ths_filename1)
        print(self.ths_0)
        print(self.ths_1)

    def _res_filename(self):
        return f"{self.proj_name}_N{self.N}_prngoff{int(self.prng_off)}_o{self.offset}_{self.label}"

    def run_ttest(self, num_traces=None, chunk=5000, normalize=False, append=True):
        if num_traces is None:
            num_traces = min(len(self.ths_0.samples), len(self.ths_1.samples))
        self.ttest = scared.ttest.TTestAnalysis() if not append or self.ttest is None else self.ttest
        start = 0 if not append else self.num_traces
        for i in tqdm(range(start, num_traces, chunk), desc='Running T-Test'):
            end = min(i + chunk, num_traces)
            if normalize:
                samples_0 = self.ths_0.samples[i:end] - np.mean(self.ths_0.samples[i:end], axis=1, keepdims=True)
                ths_0 = estraces.read_ths_from_ram(samples_0)
                samples_1 = self.ths_1.samples[i:end] - np.mean(self.ths_1.samples[i:end], axis=1, keepdims=True)
                ths_1 = estraces.read_ths_from_ram(samples_1)
                container = scared.TTestContainer(ths_0, ths_1)
            else:
                ths_0 = estraces.read_ths_from_ram(self.ths_0.samples[i:end])
                ths_1 = estraces.read_ths_from_ram(self.ths_1.samples[i:end])
                container = scared.TTestContainer(ths_0, ths_1)
            self.ttest.run(container)
        self.num_traces = num_traces

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
                plt.ylabel("t-value", fontsize=label_fontsize)
            else:
                plt.xlabel("sample index")
                plt.ylabel("t-value")
        if put_title:
            plt.title(f'T-Test Results ({self.num_traces} traces)')
        if plt_show:
            plt.show()
        return ax

    def save_ttest_results(self):
        np.save('ttest_results/'+ self.filename + '_ttest.npy', self.ttest.result)
        np.save('ttest_results/'+ self.filename + '_mean0.npy', self.mean_trace_0)
        np.save('ttest_results/'+ self.filename + '_mean1.npy', self.mean_trace_1)

    def load_ttest_results(self):
        self.ttest = scared.ttest.TTestAnalysis()
        self.ttest.result = np.load('ttest_results/'+ self.filename + '_ttest.npy')
        self.mean_trace_0 = np.load('ttest_results/'+ self.filename + '_mean0.npy')
        self.mean_trace_1 = np.load('ttest_results/'+ self.filename + '_mean1.npy')