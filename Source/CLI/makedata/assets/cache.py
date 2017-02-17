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

import sqlite3
import os


class Cache:
    """Stores signatures and preprocessed resource info"""

    def __init__(self, file_name):
        """Constructs a cache instance"""

        first_use = not os.path.exists(file_name)

        if first_use:
            os.makedirs(os.path.dirname(file_name))

        self._db = sqlite3.connect(file_name)
        self._cursor = self._db.cursor()

        if first_use:
            print('Creating cache...')
            self._query('CREATE TABLE meta(uuid text, hash text)')

    def set_meta_hash(self, uuid, value):
        """Saves resource meta file hash"""

        self._cursor.execute("SELECT hash FROM meta WHERE uuid = '%s'" % uuid)
        result = self._cursor.fetchone()

        # No cache entry - insert
        if result is None:
            self._query("INSERT INTO meta VALUES('{uuid}','{hash}')".format(uuid=uuid, hash=value))
        else:
            self._query("UPDATE meta SET hash='{hash}' WHERE uuid='{uuid}'".format(hash=value, uuid=uuid))

        self._db.commit()

    def meta_hash(self, uuid):
        """Loads resource meta file hash"""

        self._query("SELECT hash FROM meta WHERE uuid = '{uuid}'".format(uuid=uuid))
        result = self._cursor.fetchone()

        if result is None:
            return None

        return result[0]

    def update_meta_hash(self, uuid, value):
        """Updates meta hash, returns true if hash values are distinct"""

        meta_hash = self.meta_hash(uuid)

        if meta_hash == value:
            return False

        self.set_meta_hash(uuid, value)
        return True

    def _query(self, text):
        """Performs a database query"""
        self._cursor.execute(text)
