#################################################################################
#
# The MIT License (MIT)
#
# Copyright (c) 2015 Dmitry Sovetov
#
# https://github.com/dmsovetov
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#################################################################################

import threading, collections


def create(workers):
    """Creates a new task manager"""
    return Tasks(workers)


class Tasks:
    """A task manager class"""

    def __init__(self, workers):
        """Constructs a task manager instance"""
        self._workers = []
        self._current = 0
        
        for i in range(0, workers):
            self._workers.append(Worker())

    def push(self, task):
        """Pushes a new task to a worker"""
        idx = self._current % len(self._workers)
        self._workers[idx].push(task)
        self._current += 1

    def start(self):
        """Starts an action processing"""
        for w in self._workers:
            w.start()

        [w.join() for w in self._workers if w.isAlive()]


class Worker(threading.Thread):
    """Thread worker to perform an action queue."""

    def __init__(self):
        """Constructs worker instance"""

        threading.Thread.__init__(self)
        self._tasks = collections.deque()

    def push(self, task):
        """Pushes a new task to worker"""
        self._tasks.append(task)

    def run(self):
        """Runs a worker thread"""
        count = len(self._tasks)

        if count == 0:
            return

        while len(self._tasks) != 0:
            task = self._tasks.popleft()
            task()

