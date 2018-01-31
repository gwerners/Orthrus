cmd = require "cmd"
bo  = require "bo"

ts=tostring
--constantes
BO_JAVA = 1
BO_CPP  = 2
BO_LUA  = 3

function listagem()
   return "listagem executada"
end
function estatistica()
   return "estatistica executada"
end

function bogus()
   SetX(GetX() + 1)
end


--add objects for admin tasks

bo.load("objects.lua")
bo.list()
bo.remove(2)
bo.list()
bo.save("objects.lua")

--[[
print "begin lua admin test"

cmd.add('listar',listagem)
cmd.add('estatistica',estatistica)

cmd.list()
print( cmd.exec("listar") )
]]
--[[comandos para implementar:
   1 - parar servidor completamente - block incoming
   2 - derrubar servidor completamente   
   3 - bloquear objetos do servico xpto (nao derruba processos)
   4 - derrubar objetos do servico xpto (kill term nos processos)
   5 - reiniciar objetos do servico xpto
   6 - remover linha conf de servico xpto
   7 - listar linhas conf de servico
   8 - shell lua...
]]
