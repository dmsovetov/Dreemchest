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

import os, fnmatch

# Scans the directory recursively and returns the list of outdated files
def find_outdated(path):
    print 'Scanning', path

    result = {}

    for root, dirs, files in os.walk(path):
        for file_name in files:
            name = os.path.splitext(file_name)[0]
            path = os.path.join(root, file_name)

        #    if name in result:
        #        raise Exception('Duplicate file name "{0}"\n\t{1}\n\t{2}'.format(name, path, result[name]))

            result[name] = path

    return result

# Builds each file to an output folder
def build(tasks, files, path, rules):
    for file_name, file_path in files.items():
        rule = None

        for k, v in rules.items():
            if fnmatch.fnmatch(file_path, k):
                rule = v
                break

        if rule:
            tasks.push(rule(file_path, os.path.join(path, file_name)))