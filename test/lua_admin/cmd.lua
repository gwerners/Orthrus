local cmd={}

function cmd.list()
   for k, v in ipairs(cmd) do
	   print(string.format("cmd[%s](%s)",v.name,tostring(v.action)))
   end
end
function cmd.add(name,action)
   local t={name=name,action=action}
   print( "Add[" .. name .. " " .. tostring(action) .. "]")
   table.insert(cmd,t);
end
function cmd.exec(name)
   for k, v in ipairs(cmd) do
      if v.name == name then
         return v.action()
      end
   end
end

return cmd
