#include "stdafx.h"

#include "robot_conn.h"
#include "robot_user.h"
#include "robot_interact.h"

#include "cs_proto/proto_msgid.pb.h"

extern int tolua_robot_open(lua_State* tolua_S);

int32_t lua_create_user(lua_State* L)
{
	int32_t nRetCode = 0;
	CRobotUser* pUser = NULL;
	const char* pcszCoName = NULL;
	const char* pcszUserName = NULL;
	BOOL bInteractMode = FALSE;

	pcszUserName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszUserName);
	
	pcszCoName = lua_tostring(L, 2);
	LOG_PROCESS_ERROR(pcszCoName);

	bInteractMode = lua_tointeger(L, 3);

	pUser = CRobotUserMgr::instance().create_user(pcszUserName);
	LOG_PROCESS_ERROR(pUser);

	pUser->set_coname(pcszCoName);

	if (bInteractMode)
	{
		CRobotInteractMgr::instance().set_interact_user(pUser);
	}

	tolua_pushusertype(L, pUser, "CRobotUser");
	return 1;

Exit0:
	return 0;
}

int32_t lua_destroy_user(lua_State* L)
{
	int32_t nRetCode = 0;
	int32_t nUserID = 0;
	CRobotUser* pUser = NULL;

	pUser = (CRobotUser*)tolua_tousertype(L, 1, 0);
	LOG_PROCESS_ERROR(pUser);

	pUser->set_coname("");

	lua_pushboolean(L, TRUE);
	return 1;
Exit0:
	return 0;
}

int32_t lua_detach_to_user(lua_State* L)
{
	int32_t nRetCode = 0;
	CRobotUser* pUser = NULL;

	int32_t nUserID = lua_tointeger(L, 1);

	pUser = CRobotUserMgr::instance().find_user(nUserID);
	LOG_PROCESS_ERROR(pUser);

	lua_pushinteger(L, 1);
	return 1;
Exit0:
	return 0;
}


int32_t lua_check_print(lua_State *L)
{
	int32_t nRetCode = 0;
	int32_t nStatusType = 0;
	const char* pcszCaseName = NULL;
	const char* pcszContent = NULL;

	pcszCaseName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszCaseName);

	pcszContent = lua_tostring(L, 2);
	LOG_PROCESS_ERROR(pcszContent);

	nStatusType = lua_tointeger(L, 3);

    INF("case:\"%s\",content:\"%s\"", pcszCaseName, pcszContent);

Exit0:
	return 0;
}

int32_t lua_send(lua_State* L)
{
	int32_t nRetCode = 0;
	int32_t nMsgId = 0;
	int32_t nConnID = 0;
	ROBOT_CONNECTION* pConn = NULL;

	nConnID = lua_tointeger(L, 1);
	nMsgId = lua_tointeger(L, 2);

	pConn = CRobotConnMgr::instance().get_conn(nConnID);
	LOG_PROCESS_ERROR(pConn);

	nRetCode = CRobotConnMgr::instance().send(pConn, nMsgId, L);
	LOG_PROCESS_ERROR(nRetCode);
	
	lua_pushinteger(L, nRetCode);
	return 1;

Exit0:
	return 0;
}

int32_t lua_recv(lua_State* L)
{
	int32_t nRetCode = 0;
	int32_t nConnID = 0;
	std::vector<int32_t> vMsgID;

	LOG_PROCESS_ERROR(lua_gettop(L) > 0);
	nConnID = lua_tointeger(L, 1);
	lua_remove(L, 1);

	LOG_PROCESS_ERROR(nConnID);

	while (lua_gettop(L) > 0)
	{
		vMsgID.push_back(lua_tointeger(L, 1));
		lua_remove(L, 1);
	}
	nRetCode = CRobotConnMgr::instance().set_lua_wait_msgid(nConnID, vMsgID);
	LOG_PROCESS_ERROR(nRetCode);
	return 1;
Exit0:
	return 0;
}

int32_t lua_register_msgid(lua_State* L)
{
    int32_t nTable = 0;
    const google::protobuf::EnumDescriptor* pC2gEnumDesc = NULL;
    const google::protobuf::EnumDescriptor* pG2cEnumDesc = NULL;

    pC2gEnumDesc = CS_MESSAGE_ID_descriptor();
    LOG_PROCESS_ERROR(pC2gEnumDesc);

    pG2cEnumDesc = SC_MESSAGE_ID_descriptor();
    LOG_PROCESS_ERROR(pG2cEnumDesc);

    lua_newtable(L);
    nTable = lua_gettop(L);

    for (int32_t i = 0; i < pC2gEnumDesc->value_count(); i++)
    {
        const google::protobuf::EnumValueDescriptor* pValueDesc = pC2gEnumDesc->value(i);
        LOG_PROCESS_ERROR(pValueDesc);

        lua_pushstring(L, pValueDesc->name().c_str());
        lua_pushinteger(L, pValueDesc->number());
        lua_settable(L, nTable);
    }

    for (int32_t i = 0; i < pG2cEnumDesc->value_count(); i++)
    {
        const google::protobuf::EnumValueDescriptor* pValueDesc = pG2cEnumDesc->value(i);
        LOG_PROCESS_ERROR(pValueDesc);

        lua_pushstring(L, pValueDesc->name().c_str());
        lua_pushinteger(L, pValueDesc->number());
        lua_settable(L, nTable);
    }

    lua_setglobal(L, "msgid");

    DBG("lua_register_msgid success, cs count[%d] sc count[%d]", pC2gEnumDesc->value_count(), pG2cEnumDesc->value_count());

Exit0:

    return 0;
}

LUA_FUNC g_LuaFunc[] =
{
	{ "do_create_user",     lua_create_user },
	{ "do_destroy_user",    lua_destroy_user },
	{ "detach_to_user",     lua_detach_to_user },
	{ "user_send",			lua_send },  
	{ "user_recv",			lua_recv },
	{ "check_print",        lua_check_print },
    { "register_msgid",     lua_register_msgid },

	{ NULL, NULL }
};

LUA_FUNC g_PackageList[] = 
{
	{"robot.pkg", tolua_robot_open},

	{NULL, NULL}
};

