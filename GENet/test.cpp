///*
// * test.cpp
// *
// *  Created on: 2018年3月12日
// *      Author: carlos Hu
// */
//
//#include "GENet/GECrow.h"
//
//void web_test()
//{
//	new WebMgr();
//
//	CROW_ROUTE(sWeb.GetWebInstance(), "/")([]()
//	{
//		crow::json::wvalue x;
//		x["message"] = "Hello, World!";
//		x["test"] = "aaa";
//		return x;
//	});
//
//	CROW_ROUTE(sWeb.GetWebInstance(), "/add_json")
//	.methods("POST"_method, "GET"_method)([](const crow::request& req)
//	{
//		crow::json::rvalue r;
//		if(req.method == "GET"_method)
//		{
//			LOG(DEBUG)<<"IS GET"<<endl;
//			r = crow::json::load(req.url_params.get("params"));
//		}
//		else
//		{
//			LOG(DEBUG) <<"IS POST "<<req.body<<endl;
//			r = crow::json::load(req.body);
//		}
//
//		if(!r)
//		{
//			crow::json::wvalue ret;
//			ret["errorcode"] = 100;
//			ret["test"] = "aaa";
//			return ret;
//		}
//
//		crow::json::wvalue ret{r};
//		return ret;
//	});
//
//	sWeb.RunbyPort(8889);
//}
//
