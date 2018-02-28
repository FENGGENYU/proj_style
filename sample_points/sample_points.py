import sys
import os
import json
import subprocess

def sample_points(datadir,outdir):
	'''
	Load category, model names from a shapenet dataset.
	'''

	def model_names(model_path):
		""" Return model names"""
		model_names = [name for name in os.listdir(model_path)]
		return model_names

	# category = cat['name']
	models = model_names(datadir)
	
	if not os.path.exists(outdir):
		os.makedirs(outdir)
	print models
	for model_id in models:
		obj_file = os.path.join(datadir,model_id)
		print obj_file
		#subprocess.call(["meshlabserver","-i %s -o %s/model_sample.off -s 111.mlx"%(obj_file,obj_path)])
		#subprocess.call(["ls","-l"])
		subprocess.call("F:\MeshLab\meshlabserver_32.exe -i %s -o %s/%s.off -s 123.mlx"%(obj_file,outdir,model_id[0:len(model_id)-4]),shell=True)
	
if __name__ == '__main__':
	for pt in sys.argv[1:]:
		if pt[:5]=="data=":
			datadir = pt[5:]
		if pt[:4]=="out=":
			outdir = pt[4:]
		else:
			cmd = pt
	if datadir[-1]=='/':
		datadir = datadir[:-1]
	
	assert os.path.exists(datadir),"data dir not exists"
	sample_points(datadir,outdir)