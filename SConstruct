
#TODO use GLob() (Glob('src/game/client/*.cpp', True, False, True))

import sys
import os.path
import platform

arch = platform.architecture()[0]
if arch == "64bit":
	arch = str(64)
else:
	arch = str(32)

print (platform.architecture()[0])

# --- GLOBAL VARS ---
src_dir = 'src/'
bin_dir_client = 'bin/release/client/'
bin_dir_server = 'bin/release/server/'
bin_dir_test = 'bin/release/test/'

#==============================================================================
#--- Set environment ---------------------------------------------------------
#==============================================================================

#--------------------------------------
#--- MAC OS X ---
#--------------------------------------

if sys.platform == 'darwin':
	
	env_client = Environment(
		tools = ['default'],
		LINKFLAGS = ' -framework OpenGL -framework Cocoa -Wl,-rpath,@executable_path/../Frameworks',
		CPPPATH = ['/usr/local/include'],
		LIBS = ['SDL2main', 'SDL2', 'freetype', 'CEGUIOpenGLRenderer-0', 'CEGUIBase-0'],
		LIBPATH = ['libs/cegui/lib/macosx', '/usr/local/lib']
	)
		
	env_server = Environment(
		LINKFLAGS = ' -framework CoreFoundation -Wl,-rpath,@executable_path/../Frameworks',
		tools = ['default']
	)
	
	env_test = Environment(
		tools = ['default'],
		LIBS = ['gtest_main']
	)
	
#--------------------------------------
#--- WINDOWS ---
#--------------------------------------

if sys.platform == 'win32':

	env_client = Environment(
		tools = ['mingw'],
		CPPDEFINES = ['GCC_WINDRES'],
		# mingw32 indicates that program is console app and not Win32 app, so avoid undefined reference WinMain@16 problem
		LIBS = ['mingw32', 'opengl32', 'glu32', 'SDL2Main', 'SDL2', 'freetype', 'ws2_32', 'CEGUIOpenGLRenderer-0.dll', 'CEGUIBase-0.dll'], # FIXME remove dll ext
		# put the path of the MinGW lib directory for linking mingw, opengl and winsock
		LIBPATH = ['C:/MinGW/lib', 'libs/sdl/lib/windows', 'libs/freetype/lib', 'libs/cegui/lib/windows'] 
	)
		
	env_server = Environment(
		tools = ['mingw'],
		CPPDEFINES = ['GCC_WINDRES'],
		CPPPATH = ['libs/sdl/inc'],
		LIBS = ['mingw32', 'ws2_32'],
		LIBPATH = ['C:/MinGW/lib']
	)
	 
	env_test = Environment()
	
	
#--------------------------------------
#--- LINUX ----
#--------------------------------------

if sys.platform == 'linux2':

	env_client = Environment(
		CPPPATH = ['/usr/include/', 'libs/sdl/inc'],
		LIBS = ['GL', 'GLU', 'SDL2main', 'SDL2', 'freetype', 'CEGUIOpenGLRenderer-0', 'CEGUIBase-0', 'pthread', 'dl'],
		LIBPATH = ['libs/sdl/lib/linux/' + arch, 'libs/freetype/lib', 'libs/cegui/lib/linux/' + arch],
		LINKFLAGS = ' -Wl,-rpath ./lib'
	)
	
	env_server = Environment(
		LIBS = ['dl']
	)
	
	env_test = Environment()


#--------------------------------------
#--- ALL PLATFORMS ---
#--------------------------------------

# -Wall          This enables all the warnings about constructions that some users consider questionable
# -Wfatal-errors This option causes the compiler to abort compilation on the first error occurred rather than trying to keep going and printing further error messages
# -w             Inhibit all warning messages.
env_client['CC']       = env_server['CC']       = env_test['CC'] =       'gcc'
env_client['CXX']      = env_server['CXX']      = env_test['CXX'] =      'g++ -std=c++11'

if sys.platform == 'win32':
    env_client['CPPFLAGS']  = env_server['CPPFLAGS']  = env_test['CPPFLAGS'] =  '-w' # -Wuninitialized -Wfatal-errors'
    env_client.Append(LINKFLAGS = ' -m32')
    env_server.Append(LINKFLAGS = ' -m32')
    env_server.Append(LINKFLAGS = ' -m32')
else:
 	env_client['CPPFLAGS']  = env_server['CPPFLAGS']  = env_test['CPPFLAGS'] =  '-Wall' # -Wuninitialized -Wfatal-errors'

env_client['CXXFLAGS'] = env_server['CXXFLAGS'] = env_test['CXXFLAGS'] = '-DTLOG'
env_client['CPPDEFINES'] = 'GLEW_STATIC'

env_client.Append(CPPPATH = [
    'src', # internals things, just include headers with <> for absolute needs
    'libs/glew/inc', 
    'libs/freetype/inc',
    'libs/freetype/inc/freetype2',
    'libs/cegui/inc',
    'libs/sdl/inc'
])
                
env_server['CPPPATH'] = [
	'src'
]

env_test['CPPPATH'] = [
	'src',
    'libs/gtest/inc'
]

# Google test library add
env_test.Append(LIBPATH = ['libs/gtest/lib'])

# --- OPTIONS ---
AddOption('--dbg', help='Set compiler to compile for debug')
AddOption('--gprof', help='Set compiler to compile for gprof profiling')
AddOption('--arch', help='Set architecture')

if GetOption('gprof') :
	env_client.Append(CCFLAGS = ' -pg')
	env_server.Append(CCFLAGS = ' -pg')
	env_test.Append(CCFLAGS = ' -pg')
	env_client.Append(LINKFLAGS = ' -pg')
	env_server.Append(LINKFLAGS = ' -pg')
	env_test.Append(LINKFLAGS = ' -pg')
	env_server['LIBS'] = ['dl'] # capture SIGUSR1 to call gprof stuffs before exit program 
elif GetOption('dbg') :
	env_client.Append(CCFLAGS = ' -g -O0')
	env_server.Append(CCFLAGS = ' -g -O0')
	env_test.Append(CCFLAGS = ' -g -O0')
else:
	env_client.Append(CCFLAGS = ' -O2')
	env_server.Append(CCFLAGS = ' -O2')
	env_test.Append(CCFLAGS = ' -O2')


#==============================================================================
#--- Utility Functions -------------------------------------------------------
#==============================================================================

def SetDirPrefix(dir, filepaths):
	prefixedFilepaths = []
	for filepath in filepaths:
		prefixedFilepaths.append(dir + filepath) 
	return prefixedFilepaths

def PrintList(title, list):
	print (title)
	for e in list:
		print (e)

def CheckFileExisting(filepathList):
	print ('<- Check file existing ...')
	fileNotFoundCount = 0
	fileCount = 0
	for filepath in filepathList:
		if not os.path.exists(filepath):
			print ('File not found: ' + filepath)
			fileNotFoundCount += 1
    
	print (str(len(filepathList)) + ' files checked')
	
	if fileNotFoundCount == 0:
		print ('-> Check success')
	else:
		print ('-> Check fail')

def PrintBanner(title):
	print ('==================================================')
	print ('--- ' + title + ' ---')
	print ('==================================================')

def GenerateBinary(srcFilesList, targetList, doCheckFileExisting, install_dir):
	if len(targetList) > 1:
		PrintBanner('Build All')
	else:
		PrintBanner('Build ' + targetList[0])
	
	# Replace src_dir by bin_dir to have not any .o files generated into the 
	# sources file directories. This is done because VariantDir function is 
	# used (and thus generate .o files into bin directory)
	i=0
	for target in targetList:
		
		# select environment, then build binary and install it in good folder
		if target == client_target:
			bin_src_files = SetDirPrefix(bin_dir_client, srcFilesList[i])
			env_client.VariantDir(bin_dir_client, src_dir,  duplicate=0)
			bin_client = env_client.Program(client_target, bin_src_files)
			env_client.Install(install_dir, bin_client)
			env_client.Alias(client_target, install_dir)
			
		elif target == server_target :
			bin_src_files = SetDirPrefix(bin_dir_server, srcFilesList[i])
			env_server.VariantDir(bin_dir_server, src_dir,  duplicate=0)
			bin_server = env_server.Program(server_target, bin_src_files)
			env_server.Install(install_dir, bin_server)
			env_server.Alias(server_target, install_dir)
		
		elif target == test_target :
			bin_src_files = SetDirPrefix(bin_dir_test, srcFilesList[i])
			env_test.VariantDir(bin_dir_test, src_dir,  duplicate=0)
			bin_test = env_test.Program(test_target, bin_src_files)
			env_test.Install(install_dir, bin_test)
			env_test.Alias(test_target, install_dir)
			
		else:
			print ('Target ' + target + ' unknown !')
			
		i+=1



#==============================================================================
#--- Common --------------------------------------------------------
#==============================================================================

#--------------------------------------
#--- Base ---
#--------------------------------------
base_dir = 'base/'
base_src_files = [
	 'FileLoader.cpp',
	 'Logger.cpp',
	 'Ray.cpp',
	 'RayTracing.cpp',
	 'Regulator.cpp',
	 'Time.cpp',
	 'tri_logger.cpp'	 
]
base_src_files = SetDirPrefix( base_dir, base_src_files )

#--------------------------------------
#--- Engine shared ---
#--------------------------------------
shared_engine_dir = 'engine/shared/'
shared_engine_src_files = [
	'MapLoader.cpp',
	'Address.cpp',
	'Connection.cpp',
	'FlowControl.cpp',
	'Net.c',
	'NetServer.cpp',
	'NetPiece.cpp',
	'ReliabilitySystem.cpp',
	'ReliableConnection.cpp',
	'Socket.cpp',
	#'Snapshot.cpp',
	'AbsDim.cpp',
	'NetDataArray.cpp',
	'MonitoringVars.cpp',
	'ConfigLoader.cpp',
	'Huffman.cpp',
	'Snapshot.cpp'
]
shared_engine_src_files = SetDirPrefix(shared_engine_dir, shared_engine_src_files)

#--------------------------------------
#--- External ---
#--------------------------------------
external_dir = 'engine/external/'
external_src_files = [
	'glew/glew.c',
	'pnglite/pnglite.c',
	'wavpack/bits.c',
	'wavpack/metadata.c',
	'wavpack/unpack.c',
	'wavpack/words.c',
	'wavpack/wputils.c',
	'wavpack/float.c',
	'zlib/adler32.c',
	'zlib/compress.c',
	'zlib/crc32.c',
	'zlib/deflate.c',
	'zlib/gzio.c',
	'zlib/inffast.c',
	'zlib/inflate.c',
	'zlib/inftrees.c',
	'zlib/trees.c',
	'zlib/uncompr.c',
	'zlib/zutil.c'
]
external_src_files = SetDirPrefix(external_dir, external_src_files)
common_src_files = base_src_files + shared_engine_src_files

#==============================================================================
#--- Client ------------------------------------------------------------------
#==============================================================================
client_target = 'Rouage'

#--------------------------------------
#--- Main ---
#--------------------------------------
client_main_src_file = ['engine/client/Client.cpp']

#--------------------------------------
#--- Engine --- 
#--------------------------------------
client_engine_dir = 'engine/client/'
client_engine_src_files = [
	'ClientConfig.cpp',
	'NetClient.cpp',
	'Window.cpp',
	'gfx/GFXAsset.cpp',
	'gfx/VertexBufferObject.cpp',
	'InputManager.cpp',
	'gfx/VBOSprite.cpp',
	'gfx/SpriteAnimation.cpp',
	'gfx/Panel.cpp',
	'gfx/text/GLFT_Font.cpp',
	'gfx/text/TextRender.cpp',
	'sfx/SFXAsset.cpp',
	'sfx/Channel.cpp',
	'sfx/Sound.cpp',
	'sfx/SoundPlayer.cpp',
	'sfx/Voice.cpp',
	'gfx/Sprite.cpp',
	'gfx/VBO.cpp',
	'Console.cpp',
	'CommandManager.cpp',
	'ui/FormattedListboxTextItem.cpp',
	'ClientSnapshotManager.cpp'
]
client_engine_src_files = SetDirPrefix(client_engine_dir, client_engine_src_files)

#--------------------------------------
#--- Game ---
#--------------------------------------
client_game_dir = 'game/client/'
client_game_src_files = [
	'GameClient.cpp',
	'Camera.cpp',
	'Renderer.cpp',
	'MapClient.cpp',
	'Chat.cpp',
	'Particle.cpp',
	'ParticleManager.cpp',
	'Effects.cpp',
	'Giblet.cpp',
	'ui/Menu.cpp'
	# 'Crosshair.cpp'	
	# 'ui/HUD.cpp',
	# 'ui/KillMessages.cpp',
]
client_game_src_files = SetDirPrefix(client_game_dir, client_game_src_files)
client_src_files = client_main_src_file + client_engine_src_files + client_game_src_files + common_src_files + external_src_files

#==============================================================================
#--- Server ------------------------------------------------------------------
#==============================================================================
server_target = 'RouageServer'

#--------------------------------------
#--- Main ---
#--------------------------------------
server_main_src_file = [
	'engine/server/Server.cpp',
	'engine/server/ServerSnapshotManager.cpp'
]

#--------------------------------------
#--- Engine ---
#--------------------------------------
server_engine_dir = 'engine/server/'
server_engine_src_files = []
server_engine_src_files = SetDirPrefix(server_engine_dir, server_engine_src_files)

#--------------------------------------
#--- Game ---
#--------------------------------------
server_game_dir = 'game/server/'
server_game_src_files = [
	'AK47.cpp',
	'BaseGameEntity.cpp',
	'Bullet.cpp',
	'Character.cpp',
	'CollisionManager.cpp',
	'CollisionModel.cpp',
	'GameServer.cpp',
	'HealthGiver.cpp',
	'Map.cpp',
	'MovingEntity.cpp',
	'Pistol.cpp',
	'Player.cpp',
	'PlayerWeaponSystem.cpp',
	'PowerupGiver.cpp',
	'Projectile.cpp',
	'Rocket.cpp',
	'RocketLauncher.cpp',
	'Scoring.cpp',
	'Shotgun.cpp',
	'Tile.cpp',
	'Trigger.cpp',
	'TriggerRespawning.cpp',
	'TriggerSystem.cpp',
	'Weapon.cpp',
	'WeaponGiver.cpp',
	'WeaponSystem.cpp'
]
server_game_src_files = SetDirPrefix(server_game_dir, server_game_src_files)
server_src_files = server_main_src_file + server_engine_src_files + server_game_src_files + common_src_files

#==============================================================================
#--- Tests -------------------------------------------------------------------
#==============================================================================
test_target = 'RouageTest'
test_dir = 'test/'
test_src_files = [
	'engine/shared/NetPieceTest.cpp',
	'base/StatCalcTest.cpp',
	'base/BitTricksTest.cpp',
	'base/UtilsTest.cpp',
	'base/CircularBufferTest.cpp',
	'engine/shared/HuffmanTest.cpp'
	
]
test_src_files = SetDirPrefix(test_dir, test_src_files)
#test_src_files =  test_src_files + server_engine_src_files + server_game_src_files + client_engine_src_files + client_game_src_files + common_src_files
test_src_files =  test_src_files + common_src_files

#==============================================================================
#--- Build -------------------------------------------------------------------
#==============================================================================

def SetClientSourceFiles(client_src_files, server_game_src_files):
	# add sources for objective C++ wrapping for Mac OS X
	#if sys.platform == 'darwin':
	#	client_src_files += ['macosx/SDLMain.m']
	# Add some game server files because we need (Map is shared for instance) 
	client_src_files += server_game_src_files


# Custom options
#VariantDir(bin_dir, src_dir,  duplicate=0)
AddOption('--checkFileExisting', dest='doCheckFileExisting', help='Indicates if file existing check must be done')
				
# Retrieve target from command line
target = 'all'
if len(COMMAND_LINE_TARGETS) > 0:
	target = COMMAND_LINE_TARGETS[0]
	
doCheckFileExisting = False
if GetOption('doCheckFileExisting'):
	doCheckFileExisting = True    
	     
install_dir = 'bin/release'

# Build Client
if target == client_target:
	targetList = [client_target]
	SetClientSourceFiles(client_src_files, server_game_src_files)
	srcFiles = [client_src_files]
	GenerateBinary(srcFiles, targetList, doCheckFileExisting, install_dir)

# Build Server
elif target == server_target:
	targetList = [server_target]
	srcFiles = [server_src_files]
	GenerateBinary(srcFiles, targetList, doCheckFileExisting, install_dir)

# Build Test
elif target == test_target:
	targetList = [test_target]
	srcFiles = [test_src_files]
	GenerateBinary(srcFiles, targetList, doCheckFileExisting, install_dir)

# Build Client and Server
else:

	testAllowed = False #FIXME clean that
	if testAllowed:
		targetList = [client_target, server_target, test_target]
		SetClientSourceFiles(client_src_files, server_game_src_files)
		srcFiles = [client_src_files, server_src_files, test_src_files]
	else:
		targetList = [client_target, server_target]
		SetClientSourceFiles(client_src_files, server_game_src_files)
		srcFiles = [client_src_files, server_src_files]
	
	GenerateBinary(srcFiles, targetList, doCheckFileExisting, install_dir)
