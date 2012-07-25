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

#include "Gaffer/Context.h"

#include "GafferScene/SceneNode.h"

using namespace std;
using namespace Imath;
using namespace IECore;
using namespace GafferScene;
using namespace Gaffer;

IE_CORE_DEFINERUNTIMETYPED( SceneNode );

SceneNode::SceneNode( const std::string &name )
	:	Node( name )
{
	addChild( new ScenePlug( "out", Gaffer::Plug::Out ) );
}

SceneNode::~SceneNode()
{
}

ScenePlug *SceneNode::outPlug()
{
	return getChild<ScenePlug>( "out" );
}

const ScenePlug *SceneNode::outPlug() const
{
	return getChild<ScenePlug>( "out" );
}
				
void SceneNode::compute( ValuePlug *output, const Context *context ) const
{
	ScenePlug *scenePlug = output->ancestor<ScenePlug>();
	if( scenePlug )
	{
		if( output == scenePlug->boundPlug() )
		{
			std::string scenePath = context->get<std::string>( "scene:path" );
			static_cast<AtomicBox3fPlug *>( output )->setValue(
				computeBound( scenePath, context, scenePlug )
			);
		}
		else if( output == scenePlug->transformPlug() )
		{
			std::string scenePath = context->get<std::string>( "scene:path" );
			M44f transform = computeTransform( scenePath, context, scenePlug );
			if( scenePath == "/" && transform != M44f() )
			{
				throw Exception( "Scene root must have the identity transform" );
			}
			static_cast<M44fPlug *>( output )->setValue( transform );
		}
		else if( output == scenePlug->attributesPlug() )
		{
			std::string scenePath = context->get<std::string>( "scene:path" );
			CompoundObjectPtr attributes = computeAttributes( scenePath, context, scenePlug );
			if( scenePath == "/" && attributes )
			{
				throw Exception( "Scene root must have no attributes" );
			}
			static_cast<CompoundObjectPlug *>( output )->setValue( attributes );
		}
		else if( output == scenePlug->objectPlug() )
		{
			std::string scenePath = context->get<std::string>( "scene:path" );
			ObjectPtr object = computeObject( scenePath, context, scenePlug );
			if( scenePath == "/" && object )
			{
				throw Exception( "Scene root must not have an object" );			
			}
			static_cast<ObjectPlug *>( output )->setValue( object );
		}
		else if( output == scenePlug->childNamesPlug() )
		{
			std::string scenePath = context->get<std::string>( "scene:path" );
			static_cast<StringVectorDataPlug *>( output )->setValue(
				computeChildNames( scenePath, context, scenePlug )
			);
		}
		else if( output == scenePlug->globalsPlug() )
		{
			static_cast<ObjectVectorPlug *>( output )->setValue(
				computeGlobals( context, scenePlug )
			);
		}
	}
}

Imath::Box3f SceneNode::unionOfTransformedChildBounds( const ScenePath &path, const ScenePlug *out ) const
{
	Box3f result;
	ConstStringVectorDataPtr childNames = out->childNames( path );
	if( childNames )
	{
		for( vector<string>::const_iterator it = childNames->readable().begin(); it != childNames->readable().end(); it++ )
		{
			string childPath = path;
			if( childPath.size() > 1 )
			{	
				childPath += "/";
			}
			childPath += *it;
			Box3f childBound = out->bound( childPath );
			childBound = transform( childBound, out->transform( childPath ) );
			result.extendBy( childBound );
		}
	}
	return result;
}
