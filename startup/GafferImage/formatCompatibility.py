##########################################################################
#
#  Copyright (c) 2015, Image Engine Design Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import IECore

import Gaffer
import GafferImage

__currentlyLoadingScript = None

def __getSerialisedGafferVersion( scriptNode ) :

	return (
		Gaffer.Metadata.nodeValue( scriptNode, "serialiser:milestoneVersion" ),
		Gaffer.Metadata.nodeValue( scriptNode, "serialiser:majorVersion" ),
		Gaffer.Metadata.nodeValue( scriptNode, "serialiser:minorVersion" ),
		Gaffer.Metadata.nodeValue( scriptNode, "serialiser:patchVersion" )
	)

def __convertFormat( fmt ):

	if not __currentlyLoadingScript :
		return fmt

	gafferVersion = __getSerialisedGafferVersion( __currentlyLoadingScript )

	if ( gafferVersion < ( 0, 17, 0, 0 ) ) :
		displayWindow = fmt.getDisplayWindow()
		displayWindow.max += IECore.V2i( 1 )

		return GafferImage.Format( displayWindow, fmt.getPixelAspect() )

	return fmt

def __formatPlugSetValue( self, *args, **kwargs ) :

	if args and isinstance( args[0], GafferImage.Format ) :
		args = ( __convertFormat( args[0] ), ) + args[1:]

	return self.__originalSetValue( *args, **kwargs )

@staticmethod
def __formatRegisterFormat( arg1, arg2 ) :

	if isinstance( arg1, GafferImage.Format ) :
		# Old ordering of arguments.
		format = arg1
		name = arg2
	else :
		name = arg1
		format = arg2

	return GafferImage.Format.__originalRegisterFormat( name, __convertFormat( format ) )

def __scriptNodeLoad( self, *args, **kwargs ) :

	global __currentlyLoadingScript
	__currentlyLoadingScript = self

	try:
		self.__originalLoad( *args, **kwargs )
	finally:
		__currentlyLoadingScript = None

# We shouldn't need this conditional, but IECore.loadConfig() will happily load
# the same file twice if the same path has been included twice. That would cause
# havoc for us, because injecting the methods twice means infinite recursion when
# calling the "original" methods.
if not hasattr( GafferImage.Format, "__originalRegisterFormat" ) :

	Gaffer.ScriptNode.__originalLoad = Gaffer.ScriptNode.load
	Gaffer.ScriptNode.load = __scriptNodeLoad

	GafferImage.AtomicFormatPlug.__originalSetValue = GafferImage.AtomicFormatPlug.setValue
	GafferImage.AtomicFormatPlug.setValue = __formatPlugSetValue

	GafferImage.FormatPlug.__originalSetValue = GafferImage.FormatPlug.setValue
	GafferImage.FormatPlug.setValue = __formatPlugSetValue

	GafferImage.Format.__originalRegisterFormat = staticmethod( GafferImage.Format.registerFormat )
	GafferImage.Format.registerFormat = __formatRegisterFormat
