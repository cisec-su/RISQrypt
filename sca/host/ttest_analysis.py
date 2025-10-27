import scared
import numpy as np
import matplotlib.pyplot as plt


class TTestAnalysis:
    def __init__(self, ths_0, ths_1):
        self.ths_0 = ths_0
        self.ths_1 = ths_1
        self.ttest = scared.ttest.TTestAnalysis()

    def run_ttest(self, num_traces=None):
        if num_traces is None:
            num_traces = len(self.ths_0.samples)
        container = scared.TTestContainer(self.ths_0[:num_traces], self.ths_1[:num_traces])
        self.ttest.run(container)
        self.num_traces = num_traces

    def plot_means(self, plot0=True, plot1=True, put_title=True, put_legend=True, num_traces=1000):
        if num_traces < len(self.ths_0.samples):
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

    def plot_ttest(self, put_title=True, ylimit=None):
        fig, ax = plt.subplots()
        ax.plot(np.abs(self.ttest.result))
        if ylimit is not None:
            ax.set_ylim(0, ylimit)
        ax.axhline(y=4.5, color='red', linestyle='--', alpha=0.7)
        if put_title:
            plt.title(f'T-Test Results ({self.num_traces} traces)')
        plt.show()
