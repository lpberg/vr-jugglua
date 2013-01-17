require("getScriptFilename")
vrjLua.appendToModelSearchPath(getScriptFilename())

--EXAMPLE SOUND USING A WAV FILE

--find the path of your sound file
wavPath = vrjLua.findInModelSearchPath([[sample_sound.wav]]) 

--change to the correct api, wav files use OpenAL, mp3s use Audiere
snx.changeAPI("OpenAL")
--create a sound info object 
local soundInfo = snx.SoundInfo()
-- set the filename attribute of the soundFile (path to your sound file)
soundInfo.filename = wavPath
--create a new sound handle and pass it the filename from the soundInfo object
soundHandle = snx.SoundHandle(soundInfo.filename)
--configure the soundHandle to use the soundInfo
soundHandle:configure(soundInfo)
--play or "trigger" the sound
soundHandle:trigger(1)


