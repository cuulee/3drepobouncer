/**
*  Copyright (C) 2015 3D Repo Ltd
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Affero General Public License as
*  published by the Free Software Foundation, either version 3 of the
*  License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Affero General Public License for more details.
*
*  You should have received a copy of the GNU Affero General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  Mesh node
*/

#include "repo_node_mesh.h"
#include "../../../lib/repo_log.h"
using namespace repo::core::model;

MeshNode::MeshNode() :
RepoNode()
{
}

MeshNode::MeshNode(RepoBSON bson) :
RepoNode(bson)
{

}

MeshNode::~MeshNode()
{
}

std::vector<repo_color4d_t>* MeshNode::getColors() const
{
	std::vector<repo_color4d_t> *colors = new std::vector<repo_color4d_t>();
	if (hasField(REPO_NODE_LABEL_COLORS))
	{
		getBinaryFieldAsVector(getField(REPO_NODE_LABEL_COLORS), colors);
	}

	return colors;
}

std::vector<repo_vector_t>* MeshNode::getVertices() const
{
	std::vector<repo_vector_t> *vertices = new std::vector<repo_vector_t>();
	if (hasField(REPO_NODE_LABEL_VERTICES_COUNT)
		&& hasField(REPO_NODE_LABEL_VERTICES))
	{
		const uint32_t verticesSize = getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt();
		getBinaryFieldAsVector(getField(REPO_NODE_LABEL_VERTICES), verticesSize, vertices);
	}

	return vertices;
}


std::vector<repo_vector_t>* MeshNode::getNormals() const
{
	std::vector<repo_vector_t> *vertices = new std::vector<repo_vector_t>();
	if (hasField(REPO_NODE_LABEL_VERTICES_COUNT)
		&& hasField(REPO_NODE_LABEL_NORMALS))
	{
		const uint32_t verticesSize = getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt();
		getBinaryFieldAsVector(getField(REPO_NODE_LABEL_NORMALS), verticesSize, vertices);
	}

	return vertices;
}


std::vector<repo_vector2d_t>* MeshNode::getUVChannels() const
{
	std::vector<repo_vector2d_t> *channels = nullptr;
	if (hasField(REPO_NODE_LABEL_VERTICES_COUNT) 
		&& hasField(REPO_NODE_LABEL_UV_CHANNELS_COUNT)
		&& hasField(REPO_NODE_LABEL_UV_CHANNELS))
	{

		channels = new std::vector<repo_vector2d_t>();
		const uint32_t uvChannelSize = getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt() 
			* getField(REPO_NODE_LABEL_UV_CHANNELS_COUNT).numberInt();

		getBinaryFieldAsVector(getField(REPO_NODE_LABEL_UV_CHANNELS), uvChannelSize, channels);
	}

	return channels;
}

std::vector<std::vector<repo_vector2d_t>>* MeshNode::getUVChannelsSeparated() const
{
	std::vector<std::vector<repo_vector2d_t>> *channels = nullptr;

	std::vector<repo_vector2d_t> *serialisedChannels = getUVChannels();

	if (serialisedChannels)
	{
		//get number of channels and split the serialised.
		channels = new std::vector<std::vector<repo_vector2d_t>>();

		uint32_t nChannels = getField(REPO_NODE_LABEL_UV_CHANNELS_COUNT).numberInt();
		uint32_t vecPerChannel = serialisedChannels->size() / nChannels;
		channels->reserve(nChannels);
		for (uint32_t i = 0; i < nChannels; i++)
		{
			channels->push_back(std::vector<repo_vector2d_t>());
			channels->at(i).reserve(vecPerChannel);

			uint32_t offset = i*vecPerChannel;
			channels->at(i).insert(channels->at(i).begin(), serialisedChannels->begin() + offset,
				serialisedChannels->begin() + offset + vecPerChannel);

		}

		delete serialisedChannels;
	}
	return channels;
}

std::vector<repo_face_t>* MeshNode::getFaces() const
{
 //FIXME: this and the setter for faces may potentially be incorrect as the sizes of byte count aren't matching. need some looking into.
	std::vector<repo_face_t> *faces = new std::vector<repo_face_t>();
	if (hasField(REPO_NODE_LABEL_FACES)
		&& hasField(REPO_NODE_LABEL_FACES_BYTE_COUNT)
		&& hasField(REPO_NODE_LABEL_FACES_COUNT))
	{
		int32_t facesCount     = getField(REPO_NODE_LABEL_FACES_COUNT).numberInt();
		int32_t facesByteCount = getField(REPO_NODE_LABEL_FACES_BYTE_COUNT).numberInt();

		faces->resize(facesCount);
		std::vector <uint32_t> *serializedFaces = new std::vector<uint32_t>();
		
		serializedFaces->resize(facesByteCount / sizeof(uint32_t));
		getBinaryFieldAsVector(getField(REPO_NODE_LABEL_FACES), facesByteCount/sizeof(uint32_t), serializedFaces);

		// Retrieve numbers of vertices for each face and subsequent
		// indices into the vertex array.
		// In API level 1, mesh is represented as
		// [n1, v1, v2, ..., n2, v1, v2...]
		
		unsigned int counter = 0;
		int mNumIndicesIndex = 0;
		while (counter < facesCount)
		{

			if (serializedFaces->size() <= mNumIndicesIndex)
			{
				repoError << "MeshNode::getFaces() : serialisedFaces.size() <= mNumIndicesIndex!";
			}
			else
			{
				int mNumIndices = serializedFaces->at(mNumIndicesIndex);
				repo_face_t face;
				face.numIndices = mNumIndices;
				uint32_t *indices = new uint32_t[mNumIndices];
				for (int i = 0; i < mNumIndices; ++i)
					indices[i] = serializedFaces->at(mNumIndicesIndex + 1 + i);
				face.indices = indices;
				(*faces)[counter] = face;
				mNumIndicesIndex = mNumIndicesIndex + mNumIndices + 1;
			}
			
			++counter;
		}

		// Memory cleanup
		if (serializedFaces)
			delete serializedFaces;

	}

	return faces;
}