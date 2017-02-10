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

# Creates a new task manager
def create(workers):
    return Tasks(workers)

# Task manager
class Tasks:
    # ctor
    def __init__(self, workers):
        self._workers = []
        self._current = 0
        
        for i in range(0, workers):
            self._workers.append(Worker())
            
    # Pushes a new task to a worker
    def push(self, task):
        idx = self._current % len(self._workers)
        self._workers[idx].push(task)
        self._current = self._current + 1

    # Starts an action processing
    def start(self):
        for w in self._workers:
            w.start()

        [w.join() for w in self._workers if w.isAlive()]

# Thread worker to perform an action queue.
class Worker(threading.Thread):
    # ctor
	def __init__(self):
		threading.Thread.__init__(self)
		self._tasks = collections.deque()

	# Pushes a new task to worker
	def push(self, task):
		self._tasks.append(task)

	# run
	def run(self):
		count = len(self._tasks)

		if count == 0:
			return

		while len(self._tasks) != 0:
			task = self._tasks.popleft()
			task()
