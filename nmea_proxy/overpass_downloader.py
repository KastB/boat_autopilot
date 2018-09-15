import overpass
from pathlib import Path
home = str(Path.home())

api = overpass.API(debug=True)
print("query sent")
response = api.get("( \
    node(47.42437, 8.80898, 47.84266, 9.83551); \
   <; \
);",
                   responseformat="xml",
                   verbosity="meta")
print("response received - length:")
print(len(response))
with open(home + "/data/4.osm", "w") as fp:
    fp.write(response)
print("done")

# node(47.42437, 8.80898, 47.84266, 9.83551); \
#   node(47.4736, 8.8588, 47.8316, 9.7617); \
# osmconvert bodensee_new.osm -b=8.8588,47.4736,9.7617,47.8316 -o=bodensee_smaller.osm

# 1.osm : overpass mit genau der größe wie osmconver
# 2.osm : nach osmconvert
# 3.osm : alps nach osmconvert
# 4.osm : overpass bisschen größer
# 5.osm : overpass mit kleinerer größe als osmconvert

# xmllint  --exc-c14n 1.osm > 1.xml
# xmllint  --exc-c14n 2.osm > 2.xml
# diff 1.xml 2.xml
