/**
*  Copyright (C) 2016 3D Repo Ltd
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
#pragma once
#include <unordered_map>
#include "../../core/model/bson/repo_node_mesh.h"

namespace repo{
	namespace manipulator{
		namespace modelutility{
			class MeshMapReorganiser
			{
			public:
				/**
				* Construct a mesh reorganiser
				* @param mesh the mesh to reorganise
				* @param vertThreshold maximum vertices
				*/
				MeshMapReorganiser(
					const repo::core::model::MeshNode *mesh,
					const size_t                        &vertThreshold);
				~MeshMapReorganiser();


				/**
				* Return idMap arrays of the modified mesh (for material mapping)
				* @return returns a idMap arrays
				*/
				std::vector<std::vector<float>> getIDMapArrays() const {
					return idMapBuf;
				}

				/**
				* Get all mesh mapping, grouped by the sub meshes they belong to
				* @return a vector (new submeshes) of vector of mesh mappings(original submeshes)
				*/
				std::vector<std::vector<repo_mesh_mapping_t>> getMappingsPerSubMesh() const {
					return matMap;
				}

				/**
				* Get the mesh, with mesh mappings and buffers modified
				* @return returns the modified mesh
				*/
				repo::core::model::MeshNode getRemappedMesh() const;

				/**
				* Return serialised faces of the modified mesh
				* @return returns a serialised buffer of faces
				*/
				std::vector<uint16_t> getSerialisedFaces() const {
					return serialisedFaces;
				}

				/**
				* Get the mapping between submeshes UUID to new super mesh index
				* @return sub mesh to super mesh(es) mapping
				*/
				std::unordered_map<repoUUID, std::vector<uint32_t>, RepoUUIDHasher> getSplitMapping() const {
					return splitMap;
				}


			private:
				/**
				* Complete a submesh by filling in the ending parts of  mesh mapping
				* @param mapping mapping to fill in
				* @param nVertices number of vertices in this mapping
				* @param nFaces number of faces in this mapping
				* @param minBox minimum bounding box (length of 3)
				* @param maxBox maximum bounding box (length of 3)
				*/
				void finishSubMesh(
					repo_mesh_mapping_t &mapping,
					std::vector<float> &minBox,
					std::vector<float> &maxBox,
					const size_t &nVertices,
					const size_t &nFaces
					);

				/**
				* The beginning function for the whole process.
				* Merge or split submeshes where appropriate
				*/
				void performSplitting();

				/**
				* Split a single large sub mesh that exceeds the number of 
				* vertices into multiple sub meshes
				* @param currentSubMesh current sub mesh's mapping
				* @param newMappings current load of new mappings (consume and update)
				* @param idMapIdx idMap index value           (consume and update)
				* @param orgFaceIdx current face index        (consume and update)
				* @param totalVertexCount total vertice count (consume and update)
				* @param totalFaceCount total face count      (consume and update)
				*/
				void splitLargeMesh(
					const repo_mesh_mapping_t        currentSubMesh,
					std::vector<repo_mesh_mapping_t> &newMappings,
					size_t                           &idMapIdx,
					size_t                           &orgFaceIdx,
					size_t                           &totalVertexCount,
					size_t                           &totalFaceCount);

				/**
				* Start a new sub mesh
				* @param mapping mapping to fill in
				* @param meshID mesh ID
				* @param matID material ID
				* @param sVertices starting vertice #
				* @param sFaces    starting face #
				*/
				void startSubMesh(
					repo_mesh_mapping_t &mapping,
					const repoUUID      &meshID,
					const repoUUID      &matID,
					const size_t        &sVertices,
					const size_t        &sFaces
					);


				/**
				* Update the given bounding boxes given the current sub mesh's bounding boxes
				* @param min overall min bounding box to update
				* @param max overall max bounding box to update
				* @param smMin sub mesh bounding box min
				* @param smMin sub mesh bounding box max
				*/
				void updateBoundingBoxes(
					std::vector<float>  &min,
					std::vector<float>  &max,
					const repo_vector_t &smMin,
					const repo_vector_t &smMax);

				/**
				* Update the current ID Map array with the given values
				* @param n number of entires
				* @param value input value
				*/
				void updateIDMapArray(
					const size_t &n,
					const size_t &value);
				
				const repo::core::model::MeshNode *mesh;
				const size_t maxVertices;
				const std::vector<repo_vector_t> oldVertices;
				const std::vector<repo_vector_t> oldNormals;
				const std::vector<repo_face_t>   oldFaces;

				std::vector<repo_vector_t> newVertices;
				std::vector<repo_vector_t> newNormals;
				std::vector<repo_face_t>   newFaces;

				std::vector<uint16_t> serialisedFaces;

				std::vector<std::vector<float>> idMapBuf;
				std::unordered_map<repoUUID, std::vector<uint32_t>, RepoUUIDHasher> splitMap;
				std::vector<std::vector<repo_mesh_mapping_t>> matMap;
				std::vector<repo_mesh_mapping_t> reMappedMappings;


			};


		}
	}
}