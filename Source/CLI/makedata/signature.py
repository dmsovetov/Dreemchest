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

import os, hashlib, base64

# 

# Computes file hash
def file_hash(file_path):
    hash = hashlib.md5()



    return base64.b64encode(hash.digest())

# class Hash
class Hash:
	# getFileHash
	def getFileHash( self, fullPath ):
		self.beginHash()
		self.updateHash( fullPath )

		return self.endHash()

	# getFolderHash
	def getFolderHash( self, fullPath ):
		self.beginHash()

		for root, dirnames, files in os.walk( fullPath ):
			for fileName in files:
				self.updateHash( os.path.join( root, fileName ) )

		return self.endHash()

	# beginHash
#	def beginHash( self ):
#		self.hash = hashlib.md5()

	# endHash
#	def endHash( self ):
#		result = base64.b64encode(self.hash.digest())
#		self.hash = None
#		return result

	# updateHash
	def updateHash( self, fileName ):
		fh = open( fileName, 'rb' )

		while True:
			data = fh.read( 8192 )
			if not data:
				break
			self.hash.update( data )