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

#pragma once

#include "../../lib/datastructure/repo_structs.h"
#include "../../core/model/collection/repo_scene.h"

namespace repo{
	namespace manipulator{
		namespace diff{
			class AbstractDiff
			{
			public:
				/**
				* Construct a diff comparator given the 2 scenes supplied
				* @param base base scene to compare from
				* @param compare scene to compare against
				* @param gType graph type to diff
				*/
				AbstractDiff(
					const repo::core::model::RepoScene            *base,
					const repo::core::model::RepoScene            *compare,
					const repo::core::model::RepoScene::GraphType &gType
					);
				virtual ~AbstractDiff();

				/**
				* Obtain the diff result in the perspective of the base scene
				* @return return the diff result for base scene
				*/
				repo_diff_result_t getrepo_diff_result_tForBase()
				{
					return baseRes;
				}

				/**
				* Obtain the diff result in the perspective of the compare scene
				* @return return the diff result for compare scene
				*/
				repo_diff_result_t getrepo_diff_result_tForComp()
				{
					return compRes;
				}

				/**
				* Check if comparator functioned fine
				* @param msg error message should boolean returns false
				* @return returns true if comparator operated successfully
				*/
				virtual bool isOk(std::string &msg) const = 0;

			protected:
				const repo::core::model::RepoScene           *baseScene;
				const repo::core::model::RepoScene           *compareScene;
				const repo::core::model::RepoScene::GraphType gType;
				repo_diff_result_t baseRes, compRes;
			};
		}
	}
}
