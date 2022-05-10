
local function addSrcFiles( prefix )
	return prefix .. "/*cpp", prefix .. "/*.h"
end


workspace "MHP"
	configurations { "Release" }
	location "build"

	files { addSrcFiles("src") }
	files { addSrcFiles("src/manhunt") }
	files { addSrcFiles("src/render") }
	files { addSrcFiles("src/render/rw") }
	
	includedirs { "src" }
	includedirs { "src/manhunt" }
	includedirs { "inc" }

project "MHP"
	kind "SharedLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".asi"
	characterset ("MBCS")
	includedirs { os.getenv("RWSDK36") }
	links { "d3dx8" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
