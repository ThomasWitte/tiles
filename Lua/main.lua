--dofile("Lua/game.lua")

while not exit do
	if game:main_menu_dlg() == game.START then
		savefile = game:choose_savefile_dlg()
		game:load(savefile)
		game:run()
	else
		exit = true
	end
end
