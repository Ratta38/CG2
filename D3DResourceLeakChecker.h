#pragma once

#ifdef _DEBUG
class D3DResourceLeakChecker {
public:
	~D3DResourceLeakChecker();
};
#else
class D3DResourceLeakChecker {};
#endif