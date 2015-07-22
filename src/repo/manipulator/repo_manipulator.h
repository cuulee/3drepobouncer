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
* Repo Manipulator which handles all the manipulation
*/

#pragma once
#include <string>
#include "../core/handler/repo_database_handler_mongo.h"

namespace repo{
	namespace manipulator{
		class RepoManipulator
		{
		public:
			RepoManipulator();
			~RepoManipulator();

			bool connectAndAuthenticate(
				std::string       &errMsg,
				const std::string &address,
				const uint32_t         &port,
				const uint32_t    &maxConnections,
				const std::string &dbName,
				const std::string &username,
				const std::string &password,
				const bool        &pwDigested = false
				);

			core::model::bson::RepoBSON* createCredBSON(
				const std::string &databaseAd,
				const std::string &username, 
				const std::string &password, 
				const bool        &pwDigested)
			{
				core::model::bson::RepoBSON* bson = 0;

				repo::core::handler::AbstractDatabaseHandler* handler =
					repo::core::handler::MongoDatabaseHandler::getHandler(databaseAd);
				if (handler)
					bson = handler->createBSONCredentials(username, password, pwDigested);
				
				return bson;
			}


			//FIXME: need to use the credentials!
			std::list<std::string> fetchDatabases(
				const std::string                             &databaseAd, 
				const repo::core::model::bson::RepoBSON*	  &cred    
				)
			{
				std::list<std::string> list;
				repo::core::handler::AbstractDatabaseHandler* handler =
					repo::core::handler::MongoDatabaseHandler::getHandler(databaseAd);
				if(handler)
					list = handler->getDatabases();

				return list;
			}

			std::list<std::string> fetchCollections(
				const std::string                             &databaseAd,
				const repo::core::model::bson::RepoBSON*	  &cred,
				const std::string                             &database
				)
			{

				std::list<std::string> list;
				repo::core::handler::AbstractDatabaseHandler* handler =
					repo::core::handler::MongoDatabaseHandler::getHandler(databaseAd);
				if (handler)
					list = handler->getCollections(database);

				return list;
			}
		};
	}
}
