local bo={}
local myobjects={}

function AddBusinessObject(name,_type,path,instances,worker)
   print("ADD BO[".. name .. "," .. ts(_type) .. "," .. path .. ","
    .. ts(instances) .. "," .. worker ..  "]" )
   bo.add(name,_type,path,instances,worker)
end

function bo.load(filename)
   dofile(filename)
end
function bo.save(filename)
   print "Saving Objects..."
   file=open(filename,w)
   for k, v in ipairs(myobjects) do
      print(ts(k) .."->" .. v.name)
      print("\t" .. typename[v._type] )
      print("\t" .. v.path )
      if v.instances == 0 then
         print("\t" ..  "== number of CPU cores")
      else
         print("\t" ..  v.instances )
      end
      print("\t" .. v.worker )
   end
end
function bo.add(name,_type,path,instances,worker)
   local t = {
      name=name,
      _type=_type,
      path=path,
      instances=instances,
      worker=worker
   }
   print("bo.add(".. name .. "," .. ts(_type) .. "," .. path .. ","
    .. ts(instances) .. "," .. worker ..  ")" )
   table.insert(myobjects,t)
end

function bo.list()
   local typename={
   [1] = "JAVA",
   [2] = "C++",
   [3] = "LUA"
   }
   print "Registered Objects:"
   for k, v in ipairs(myobjects) do
      print(ts(k) .."->" .. v.name)
      print("\t" .. typename[v._type] )
      print("\t" .. v.path )
      if v.instances == 0 then
         print("\t" ..  "== number of CPU cores")
      else
         print("\t" ..  v.instances )
      end
      print("\t" .. v.worker )
   end
end
function bo.remove(name)
   table.remove(myobjects,name)
end
return bo
