import scared
import numpy as np
import matplotlib.pyplot as plt
from tqdm.auto import tqdm


class TTestAnalysis:
    def __init__(self, ths_0, ths_1):
        self.ths_0 = ths_0
        self.ths_1 = ths_1
        self.ttest = scared.ttest.TTestAnalysis()

    def run_ttest(self, num_traces=None, chunk=5000):
        if num_traces is None:
            num_traces = len(self.ths_0.samples)
        for i in tqdm(range(0, num_traces, chunk), desc='Running T-Test'):
            end = min(i + chunk, num_traces)
            container = scared.TTestContainer(self.ths_0[i:end], self.ths_1[i:end])
            self.ttest.run(container)
        self.num_traces = num_traces

    def plot_means(self, plot0=True, plot1=True, put_title=True, put_legend=True, num_traces=None):
        if num_traces is None or num_traces > len(self.ths_0.samples):
            num_traces = len(self.ths_0.samples)
        step = len(self.ths_0.samples) // num_traces
        if len(self.ths_0.samples) % num_traces != 0:
            step += 1
        fig, ax = plt.subplots()
        if plot0:
             mean_trace_0 = np.mean(self.ths_0.samples[::step], axis=0)
             ax.plot(mean_trace_0, label='Class 0 Mean')
        if plot1:
             mean_trace_1 = np.mean(self.ths_1.samples[::step], axis=0)
             ax.plot(mean_trace_1, label='Class 1 Mean')
        if put_legend:
            fig.legend()
        if put_title:
            plt.title('Mean Traces')

    def plot_traces(self, num_traces=5, put_title=True):
        fig, ax = plt.subplots()
        for i in range(10):
            ax.plot(self.ths_0.samples[i], color='blue', alpha=0.3)
            ax.plot(self.ths_1.samples[i], color='orange', alpha=0.3)
        if put_title:
            plt.title(f'Sample Traces (First {num_traces} from each class)')
        plt.show()        

    def plot_ttest(self, put_title=True, ylimit=None, ax=None, color='black', plt_show=True, hline=True, label_fontsize=None, put_label=True):
        if ax is None:
            fig, ax = plt.subplots()
        ax.plot(np.abs(self.ttest.result), color=color)
        if ylimit is not None:
            ax.set_ylim(0, ylimit)
        if hline:
            ax.axhline(y=4.5, color='red', linestyle='--', alpha=0.7)

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
