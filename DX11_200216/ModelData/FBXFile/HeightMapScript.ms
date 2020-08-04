-- (c)2001 KOG Inc.
-- write Heightmap data file from Plane(Editable_Mesh)
-- parameters: object
-- nWid, nLen - # of width and height vertices respectively.
-- written by ctkim@kogsoft.com

fn xptHeightMap object nWid nLen = (

--s = getSaveFileName caption:"Save HeightMap" filename:"heightmap.hm" \
-- types:"HeightMap(*.hm)|*.hm"
s = "C:\\Users\\ljm47\\Desktop\\" + object.name + ".hm"

if s == undefined then (
print "CANCELED: xptHeightMap()"
return -1
) -- if s
try (
fs = createFile s

-- write Header

-- orign's position
v = getVert object 1
format "%\n" v.x to:fs
format "%\n" v.y to:fs

-- # of vertices
format "%\n" nWid to:fs
format "%\n" nLen to:fs

-- terrain's size
v = object.max - object.min
format "%\n" v.x to:fs
format "%\n" v.y to:fs

nVerts = getNumVerts object
for i = 1 to nVerts do (
v = in coordSys object.transform getVert object i
format "%\n" v.z to:fs
) -- for i

flush fs
close fs
) catch (
print "ERROR : xptHeightMap"
return -1
)
print "OK : xptHeightMap()"
) -- fn xptHeightMap()