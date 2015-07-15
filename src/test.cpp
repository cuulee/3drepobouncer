#include <iostream>
#include <list>
#include <sstream>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "manipulator/graph/repo_graph_scene.h"
#include "manipulator/modelcreator/repo_model_creator_assimp.h"

#include "core/handler/repo_database_handler_mongo.h"

#include "core/model/repo_node_utils.h"
#include "core/model/bson/repo_bson_factory.h"
#include "core/model/bson/repo_node.h"


void configureLogging(){

	boost::log::core::get()->set_filter
	(
		boost::log::trivial::severity >= boost::log::trivial::info //only show info and above
	);
}

void testDatabaseRetrieval(repo::core::handler::AbstractDatabaseHandler *dbHandler){

	BOOST_LOG_TRIVIAL(debug) << "Retrieving list of databases...";
	std::list<std::string> databaseList = dbHandler->getDatabases();

	BOOST_LOG_TRIVIAL(debug) << "# of database: " << databaseList.size();

	std::list<std::string>::const_iterator iterator;
	for (iterator = databaseList.begin(); iterator != databaseList.end(); ++iterator) {
		BOOST_LOG_TRIVIAL(debug) << "\t" << *iterator;
	}

	BOOST_LOG_TRIVIAL(debug) << "Retrieving list of database collections";
	std::map<std::string, std::list<std::string>> mapDBProjects = dbHandler->getDatabasesWithProjects(databaseList, "scene");

	std::map<std::string, std::list<std::string>>::const_iterator mapIterator;
	for (mapIterator = mapDBProjects.begin(); mapIterator != mapDBProjects.end(); ++mapIterator) {
		BOOST_LOG_TRIVIAL(debug) << mapIterator->first << ":";
		std::list<std::string> projectList = mapIterator->second;

		for (iterator = projectList.begin(); iterator != projectList.end(); ++iterator) {
			BOOST_LOG_TRIVIAL(debug) << "\t" << *iterator;
		}
	}
}

void connect(repo::core::handler::AbstractDatabaseHandler *dbHandler, std::string username, std::string password){


	BOOST_LOG_TRIVIAL(debug) << "Connecting to database...";


	BOOST_LOG_TRIVIAL(debug) << (!dbHandler ? "FAIL" : "success!");

	BOOST_LOG_TRIVIAL(debug) << "Authenticating...";
	BOOST_LOG_TRIVIAL(debug) << (!dbHandler->authenticate(username, password) ? "FAIL" : "success!");

}

void insertARepoNode(repo::core::handler::AbstractDatabaseHandler *dbHandler){
	//namespace to insert the node into.
	std::string database = "test";
	std::string collection = "test.scene";

	//builds a repo node and dumps it out to check its value

	repo::core::model::bson::RepoNode *node = repo::core::model::bson::RepoBSONFactory::makeRepoNode("<crazy>");

	BOOST_LOG_TRIVIAL(info) << "Repo Node created: ";
	BOOST_LOG_TRIVIAL(info) << "\t" << node->toString();

	BOOST_LOG_TRIVIAL(info) << "Repo Node created inserting it in " << database << "." << collection;
	dbHandler->insertDocument(database, collection, *node);


}

void instantiateProject(repo::core::handler::AbstractDatabaseHandler *dbHandler){
	repo::manipulator::graph::SceneGraph *scene = new repo::manipulator::graph::SceneGraph(dbHandler, "test", "bridge");
	std::string errMsg;

	//scene->setRevision(stringToUUID("a79ae12a-9828-468b-804c-149f6cb835a5"));
	
	if (!scene->loadRevision(errMsg)){
		BOOST_LOG_TRIVIAL(info) << "load Revision failed " << errMsg;
	}
	else{
		BOOST_LOG_TRIVIAL(info) << "Revision loaded";
	}

	if (!scene->loadScene(errMsg)){
		BOOST_LOG_TRIVIAL(info) << "load Scene failed " << errMsg;
	}
	else{
		BOOST_LOG_TRIVIAL(info) << "Scene loaded";
	}

	std::stringstream		stringMaker;
	scene->printStatistics(stringMaker);

	std::cout << stringMaker.str();



}

void loadModelFromFile()
{
	repo::manipulator::modelcreator::AbstractModelCreator *modelCreator;

	modelCreator = new repo::manipulator::modelcreator::AssimpModelCreator();

	std::string errMsg, fileName;

	fileName = "C:\\Users\\Carmen\\Desktop\\A556-CAP-7000-S06-IE-S-1001.ifc";
	if (modelCreator->importModel( fileName ,errMsg))
	{
		BOOST_LOG_TRIVIAL(info) << "model loaded successfully! Attempting to port to Repo World...";
		repo::manipulator::graph::SceneGraph *graph = modelCreator->generateSceneGraph();

		BOOST_LOG_TRIVIAL(info) << " success? printing graph statistics...";
		std::stringstream		stringMaker;
		graph->printStatistics(stringMaker);
		std::cout << stringMaker.str();

	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "failed to load model from file : " << fileName << " : " << errMsg;
	}
}

int main(int argc, char* argv[]){

	//TODO: configuration needs to be done properly, but hey, i'm just a quick test!
	if (argc != 5){
		std::cout << "Usage: " << std::endl;
		std::cout << "\t " << argv[0] << " address port username password" << std::endl;
		return EXIT_FAILURE;
	}

	std::string address = argv[1];
	int port = atoi(argv[2]);
	std::string username = argv[3];
	std::string password = argv[4];


	repo::core::handler::AbstractDatabaseHandler *dbHandler = repo::core::handler::MongoDatabaseHandler::getHandler(address, port);

	configureLogging();

	connect(dbHandler, username, password);
	//testDatabaseRetrieval(dbHandler);

	//insertARepoNode(dbHandler);

	instantiateProject(dbHandler);

	loadModelFromFile();

	return EXIT_SUCCESS;
}
