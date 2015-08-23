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

import sqlite3, os

# Stores signatures and preprocessed resource info
class Cache:
    # ctor
    def __init__(self, file_name):
        first_use = not os.path.exists(file_name)

        self._db = sqlite3.connect(file_name)
        self._cursor = self._db.cursor()

        if first_use:
            print 'Creating cache...'
            self._query('CREATE TABLE resources(key text, data text)')

    # Saves resource info to cache
    def save(self, key, data):
        self.dbCursor.execute("SELECT data FROM resources WHERE key = '%s'" % key)
        result = self._cursor.fetchone()

        # No cache entry - insert
        if result == None:
            self._query("INSERT INTO resources VALUES('{0}', '{1}')".format(key, data))
        else:
            self._query("UPDATE resources SET key='{0}' WHERE data='{1}'".format(key, data))

        self._db.commit()

    # Loads resource info from cache
    def load(self, key):
        self._query("SELECT data FROM resources WHERE key = '%s'" % key)
        result = self._cursor.fetchone()

        return result[0] if result else None

    # Performs a database query
    def _query(self, text):
        self._cursor.execute(text)