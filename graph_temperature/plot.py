#!/usr/bin/env python

"""A colelction of classes for high-quality, dynamically
updated XY-plots.

"""

# Generic imports
import matplotlib.pyplot as plt
import collections as col
import numpy as np

# Assignment specific imports
from dispatch import TIME_INTERVALS, MINUTE_S


class Demuxer(object):
    """This class is assignment-specific."""
    def __init__(self, plots_spec):
        self.w = Window(plots_spec=plots_spec)
        self._counter_samples = 0

    def handle_new_value(self, val):
        """Update relevant plots."""
        self._counter_samples += 1
        for i, v in enumerate(TIME_INTERVALS):
            if self._counter_samples % v == 0:
                if v == MINUTE_S:
                    val = val
                else:
                    val = self._get_average(plot_num=i-1)
                self.w.add_datapoint(plot_number=i,
                                     y=val,
                                     )

    def _get_average(self, plot_num):
        data = self.w.get_yaxis(plot_num=plot_num)
        average = np.mean(data)
        return average

class Window(object):
    """Holds a collection of equally-sized, static x-axis, dynamic
     y-axis plots. Temperature range aware. Takes up the whole screen.

    """
    def __init__(self, plots_spec):
        """plots_spec - list of dicts:
            x_range, y_range, num_points

        """
        # Redraw plots as soon as self.fig.canvas.draw() is called.
        plt.ion()

        # Create the window surface
        dpi=80  # default value
        screen = get_screen_resolution()
        width = screen[0] / dpi
        height = screen[1] / dpi
        self.fig = plt.figure(figsize=(width, height), dpi=dpi)  # the main window

        # Create the individual plots
        self.plots = []
        plots_num = len(plots_spec)
        half = plots_num / 2
        plots_map = half * 100 +  (plots_num - half) * 10  # 990 to 110, 0 is the current plot
        for i, p in enumerate(plots_spec):
            def get_axis(min, max, points):
                a = np.linspace(min, max, points)
                return [int(round(f)) for f in a]

            x_axis = get_axis(p['x_range'][0], p['x_range'][1], p['num_points'])
            y_axis = get_axis(p['y_range'][0], p['y_range'][1], p['num_points'])
            graph = Graph(window=self.fig, subplot_num=plots_map + i + 1,  # add last digit
                          x_axis=x_axis, y_axis=y_axis,
                          )
            self.plots.append(graph)

    # Redrawing belongs here for fine control over this time-consuming operation.
    # Note that fig.canvas.draw() redraws the whole window!
    def update_figure(self, plot_number, y_data):
        self.plots[plot_number].update_figure(y_data)
        self.fig.canvas.draw()

    def add_datapoint(self, plot_number, y):
        self.plots[plot_number].add_datapoint(y)
        self.fig.canvas.draw()

    def get_yaxis(self, plot_num):
        return self.plots[plot_num].y_data


class Graph(object):
    """Single 2-D dynamic plot. The axes must be same length and
    have both minimum and maximum possible values.

    """
    def __init__(self, window, subplot_num, x_axis, y_axis):
        # Draw self
        ax = window.add_subplot(subplot_num)

        # Obtain handle to y-axis
        line, = ax.plot(x_axis, y_axis) # set here axis ranges
        self.y = line

        # Remember list of datapoints
        self.y_data = col.deque(y_axis,
                                maxlen=len(y_axis))  # circular buffer

        # Make plot prettier
        plt.grid(True)
        plt.tight_layout()

    def update_figure(self, new_y_data):
        self.y_data = new_y_data
        self.y.set_ydata(self.y_data)  # pyPlot call

    def add_datapoint(self, y):
        self.y_data.append(y)  # remember - circular buffer
        self.y.set_ydata(self.y_data)


### General graphical utility calls. ###
def get_screen_resolution():
    """Returns current width, height in pixels."""
    return 1366, 768


def main():
    from dispatch import MAX_TEMP, MIN_TEMP
    if 0:
        """Test just Graph class."""
        plt.ion()
        fig = plt.figure(figsize=(15,9)) # dpi == 80
        p = Graph(window=fig, subplot_num=111,
                  x_axis=range(MIN_TEMP, MAX_TEMP, 1000),
                  y_axis=range(MIN_TEMP, MAX_TEMP, 1000),
                  )
        for j in range(1, 5):
            print "Run", j
            for i in range(0, 32768, 1000):  # about half of the maximum
                p.add_datapoint(i)
                fig.canvas.draw()

    if 1:
        """Test whole window."""
        import dispatch as conv
        import random
        print conv.TIME_INTERVALS
        DATAPOINTS_PER_GRAPH = 200
        PLOTS_SPEC = [dict(x_range=(0, d),
                           y_range=(MIN_TEMP, MAX_TEMP),
                           num_points=DATAPOINTS_PER_GRAPH)
                      for d in conv.TIME_INTERVALS
                      ]

        w = Window(plots_spec=PLOTS_SPEC)
        for i in range(1, 10):
            print "Run", i
            for p in range(len(PLOTS_SPEC)):
                w.add_datapoint(plot_number=p,
                                y=random.randint(MIN_TEMP, MAX_TEMP))


if __name__ == "__main__":
    main()
