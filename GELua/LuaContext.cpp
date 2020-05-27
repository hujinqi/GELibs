#include "LuaContext.h"

LuaContext::LuaContext(bool openDefaultLibs)
{
	// luaL_newstate can return null if allocation failed
	mState = luaL_newstate();
	if (mState == nullptr)
		throw std::bad_alloc();

	// setting the panic function
	lua_atpanic(mState, [](lua_State* state) -> int {
		const std::string str = lua_tostring(state, -1);
		lua_pop(state, 1);
		assert(false && "lua_atpanic triggered");
		exit(0);
	});

	// opening default library if required to do so
	if (openDefaultLibs)
		luaL_openlibs(mState);
}

LuaContext::LuaContext(LuaContext&& s) :
	mState(s.mState)
{
	s.mState = luaL_newstate();
}

LuaContext::~LuaContext()
{
	assert(mState);
	lua_close(mState);
}

auto LuaContext::createThread()
	-> ThreadID
{
	ThreadID result;

	result.state = lua_newthread(mState);
	result.threadInRegistry = std::unique_ptr<ValueInRegistry>(new ValueInRegistry(mState));
	lua_pop(mState, 1);

	return std::move(result);
}

void LuaContext::destroyThread(ThreadID& id)
{
	id.threadInRegistry.reset();
}

void LuaContext::checkTypeRegistration(lua_State* state, const std::type_info* type)
{
	lua_pushlightuserdata(state, const_cast<std::type_info*>(type));
	lua_gettable(state, LUA_REGISTRYINDEX);
	if (!lua_isnil(state, -1)) {
		lua_pop(state, 1);
		return;
	}
	lua_pop(state, 1);

	lua_pushlightuserdata(state, const_cast<std::type_info*>(type));
	lua_newtable(state);

	lua_pushinteger(state, 0);
	lua_newtable(state);
	lua_settable(state, -3);

	lua_pushinteger(state, 1);
	lua_newtable(state);
	lua_settable(state, -3);

	lua_pushinteger(state, 3);
	lua_newtable(state);
	lua_settable(state, -3);

	lua_pushinteger(state, 4);
	lua_newtable(state);
	lua_settable(state, -3);

	lua_settable(state, LUA_REGISTRYINDEX);
}

void LuaContext::luaError(lua_State* state)
{
	lua_error(state);
	assert(false);
	std::terminate();   // removes compilation warning
}

PushedObject LuaContext::load(lua_State* state, std::istream& code) {
	// since the lua_load function requires a static function, we use this structure
	// the Reader structure is at the same time an object storing an istream and a buffer,
	//   and a static function provider
	struct Reader {
		Reader(std::istream& str) : stream(str) {}
		std::istream&           stream;
		std::array<char,512>    buffer;

		// read function ; "data" must be an instance of Reader
		static const char* read(lua_State* l, void* data, size_t* size) {
			assert(size != nullptr);
			assert(data != nullptr);
			Reader& me = *static_cast<Reader*>(data);
			if (me.stream.eof())    { *size = 0; return nullptr; }

			me.stream.read(me.buffer.data(), me.buffer.size());
			*size = static_cast<size_t>(me.stream.gcount());    // gcount could return a value larger than a size_t, but its maximum is me.buffer.size() so there's no problem
			return me.buffer.data();
		}
	};

	// we create an instance of Reader, and we call lua_load
	Reader reader{code};
	const auto loadReturnValue = lua_load(state, &Reader::read, &reader, "chunk"
#           if LUA_VERSION_NUM >= 502
			, nullptr
#           endif
	);

	// now we have to check return value
	if (loadReturnValue != 0) {
		// there was an error during loading, an error message was pushed on the stack
		const std::string errorMsg = lua_tostring(state, -1);
		lua_pop(state, 1);
		if (loadReturnValue == LUA_ERRMEM)
			throw std::bad_alloc();
		else if (loadReturnValue == LUA_ERRSYNTAX)
			throw SyntaxErrorException{errorMsg};
		throw std::runtime_error("Error while calling lua_load: " + errorMsg);
	}

	return PushedObject{state, 1};
}

PushedObject LuaContext::load(lua_State* state, const char* code) {
	auto loadReturnValue = luaL_loadstring(state, code);

	// now we have to check return value
	if (loadReturnValue != 0) {
		// there was an error during loading, an error message was pushed on the stack
		const std::string errorMsg = lua_tostring(state, -1);
		lua_pop(state, 1);
		if (loadReturnValue == LUA_ERRMEM)
			throw std::bad_alloc();
		else if (loadReturnValue == LUA_ERRSYNTAX)
			throw SyntaxErrorException{errorMsg};
		throw std::runtime_error("Error while calling lua_load: " + errorMsg);
	}

	return PushedObject{state, 1};
}
