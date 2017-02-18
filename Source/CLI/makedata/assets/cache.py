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
        db_dir = os.path.dirname(file_name)

        if first_use and not os.path.exists(db_dir):
            os.makedirs(db_dir)

        self._db = sqlite3.connect(file_name)
        self._cursor = self._db.cursor()

        if first_use:
            print('Creating cache...')
            self._query('CREATE TABLE hash(uuid text, meta text, file text)')

    def set_asset_hash(self, uuid, meta_hash, file_hash):
        """Saves resource meta file hash"""

        self._cursor.execute("SELECT meta, file FROM hash WHERE uuid = '%s'" % uuid)
        result = self._cursor.fetchone()

        # No cache entry - insert
        if result is None:
            self._query("INSERT INTO hash VALUES('{uuid}','{meta}', '{file}')"
                        .format(uuid=uuid, meta=meta_hash, file=file_hash))
        else:
            self._query("UPDATE hash SET meta='{meta}', file='{file}' WHERE uuid='{uuid}'"
                        .format(meta=meta_hash, file=file_hash, uuid=uuid))

        self._db.commit()

    def asset_hash(self, uuid):
        """Loads resource meta file hash"""

        self._query("SELECT meta, file FROM hash WHERE uuid = '{uuid}'".format(uuid=uuid))
        result = self._cursor.fetchone()
        return result

    def update_asset_hash(self, uuid, meta_hash, file_hash):
        """Updates meta hash, returns true if hash values are distinct"""

        asset_hash = self.asset_hash(uuid)

        if asset_hash and meta_hash == asset_hash[0] and file_hash == asset_hash[1]:
            return False

        self.set_asset_hash(uuid, meta_hash=meta_hash, file_hash=file_hash)
        return True

    def _query(self, text):
        """Performs a database query"""
        self._cursor.execute(text)
