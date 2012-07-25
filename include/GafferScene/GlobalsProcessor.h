//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2012, John Haddon. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#ifndef GAFFERSCENE_GLOBALSPROCESSOR_H
#define GAFFERSCENE_GLOBALSPROCESSOR_H

#include "GafferScene/SceneProcessor.h"

namespace GafferScene
{

/// The GlobalsProcessor class provides a base class for modifying the globals
/// of a scene while passing everything else through unchanged.
class GlobalsProcessor : public SceneProcessor
{

	public :

		GlobalsProcessor( const std::string &name=staticTypeName() );
		virtual ~GlobalsProcessor();

		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( GlobalsProcessor, GlobalsProcessorTypeId, SceneProcessor );
		
		/// Implemented so that each child of inPlug() affects the corresponding child of outPlug()
		virtual void affects( const Gaffer::ValuePlug *input, AffectedPlugsContainer &outputs ) const;
				
	protected :
		
		/// Implemented as pass throughs.
		virtual Imath::Box3f computeBound( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const;
		virtual Imath::M44f computeTransform( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const;
		virtual IECore::CompoundObjectPtr computeAttributes( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const;
		virtual IECore::ObjectPtr computeObject( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const;
		virtual IECore::StringVectorDataPtr computeChildNames( const ScenePath &path, const Gaffer::Context *context, const ScenePlug *parent ) const;

		/// Implemented to call processGlobals.
		virtual IECore::ObjectVectorPtr computeGlobals( const Gaffer::Context *context, const ScenePlug *parent ) const;
		
		/// Must be implemented by derived classes.
		virtual IECore::ObjectVectorPtr processGlobals( const Gaffer::Context *context, IECore::ConstObjectVectorPtr inputGlobals ) const = 0;
		
};

} // namespace GafferScene

#endif // GAFFERSCENE_GLOBALSPROCESSOR_H
