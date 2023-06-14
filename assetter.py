import os
# from PyQt5 import Qt

resource_path = "/home/vandi/code/menga/Memory_Manager/raw_resources"
asset_path = "/home/vandi/code/menga/Memory_Manager/assets"
for i in os.listdir(resource_path):
	# if img.format() == QImage.Format_Indexed8:
	# 	img.setColorTable(palette)
	# else:
	os.chdir("/home/vandi/code/menga/MES/gpu/image2mes")
	# print(f"cargo run -- -i {resource_path+'/'+i[:-4]+'_'+i[-4:]} -o {resource_path+'/'+i[:-4]}.m3if -f")
	os.system(f"cargo run -- -i \"{resource_path+'/'+i[:-4]+'.png'}\" -c $TEXTURE_MAP -o {asset_path+'/'+i[:-4]}.m3if -p")
os.chdir("/home/vandi/code/menga/Memory_Manager")
os.system("mvm assets")
