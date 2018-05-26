import overpass
from pathlib import Path
home = str(Path.home())

api = overpass.API(debug=True)
print("query sent")
response = api.get("( \
   node(47.4736, 8.8588, 47.8316, 9.7617); \
   <; \
);",
                   responseformat="xml",
                   verbosity="meta")
print("response received - length:")
print(len(response))
with open(home + "/data/overpass_result.osm", "w") as fp:
    fp.write(response)
print("done")

# node(47.42437, 8.80898, 47.84266, 9.83551); \
# osmconvert bodensee_new.osm -b=8.8588,47.4736,9.7617,47.8316 -o=bodensee_smaller.osm
