import pylab
import numpy
import scipy
import random
import matplotlib as mpl
import matplotlib.pyplot as plt
import itertools as itr
import statUtils 

class dia1P_dataStream:
	def __init__(self,fName,simType='Dux'):
		"""
		simType:
			Dux (duxbury) all strengths 1
			Dist	(distribution) distribution of strengths
		"""
		self.inFile = open(fName,'r')
		self.hasData = True		# Remains true so long as there is more data in the stream.
		## Skip first three lines of the input file
		self.inFile.readline()
		ln = self.inFile.readline()
		self.inFile.readline()

		metaD = ln.split()

		self.sysSize = int(metaD[1])
		self.simType = simType

	def getSysSize(self):
		return self.sysSize

	def getSimData(self):
		"""	
		Reads a full simulation and returns data in a dictionary.
		An empty dictionary is returned if there is no more data in the file.
		"""
		data = dict()
		endOfSim = False			# Variable indicating if the end of simulation has been reached.

		# Lists to store data.		
		node1 = []
		node2 = []
		failCurr = []
		failVol = []
		fuseStrength = []

		while self.hasData and not endOfSim:
			ln = self.inFile.readline()
			if ln == '':		# Reached end of file
				self.hasData = False
			else:
				inp = ln.split()
				if int(inp[0]) == int(inp[1]):		# Reached the end of simulation.
					endOfSim = True
				else:
					node1.append(int(inp[0]))
					node2.append(int(inp[1]))
					failVol.append(float(inp[2]))
					failCurr.append(float(inp[3]))
					if self.simType == 'Dux':
						fuseStrength.append(1.0)
					elif self.simType == 'Dist':
						fuseStrength.append(float(inp[4]))
		
		if len(node1) > 0:
			data['node1'] = numpy.array(node1)
			data['node2'] = numpy.array(node2)
			data['failVol'] = numpy.array(failVol)
			data['failCurr'] = numpy.array(failCurr)
			data['fuseStrength'] = numpy.array(fuseStrength)

		return data

	def finalize(self):
		self.inFile.close()

	def __del__(self):
		self.finalize()
		

########### WRITING CLUSTERS TO FILES #############

def dia1P_writeClus(pf,L,cList):
	"""
	Writes all the nodes in the list of clusters to the given file
	"""
	for cluster in cList:
		for site in cluster:
			node = dia1P_dualLatticeToNode(site[0],site[1],L)
			n1 = max(node)
			n2 = min(node)
			pf.write(str(n1+1) + '\t' + str(n2+1) + '\n')

	pf.write('0\t0\n')

########### STATISTICAL FUNCITONS #################

def dia1P_dualClusterDist(fName,statType='S',critical=False):
	"""
	Returns the cluster numbers on the dual lattice.
	critical:
		False = Cluster numbers for full simulation
		True = Cluster numbers at max current
	statType: 
		S = Cluster number on cluster sizes
		W = Cluster number on cluster widths
		H = Cluster number on cluster heights
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	L = inStream.getSysSize()

	sizes = dict()
	
	while inStream.hasData:
		data = inStream.getSimData()			# Get data for one simulation
		if len(data) > 0:
			if critical == True:
				# Find the position of the maximum current
				maxCurr = max(data['failCurr'])
				ind = list(data['failCurr']).index(maxCurr)
				# Find clusters till max current
				clusters = dia1P_dualClusters(data['node1'][0:ind+1]-1,data['node2'][0:ind+1]-1,L)
			else:
				clusters = dia1P_dualClusters(data['node1']-1,data['node2']-1,L)		# Make clusters

			for clus in clusters:
				sz = dia1P_dualClusterSize(clus,L,statType=statType)							# Calculate the requested stat on the cluster
				if sizes.has_key(sz):
					sizes[sz] = sizes[sz] + 1
				else:
					sizes[sz] = 1	
	
	clusSize = numpy.array(sizes.keys())
	clusSize.sort()
	clusNum = numpy.zeros(len(clusSize))
	for j in range(len(clusSize)):
		clusNum[j] = sizes[clusSize[j]]
		
	return clusSize, clusNum

def dia1P_dualWidthStress(fName):
	"""
	Returns the width of the critical cluster, the critical stress and the strength of the critical fuse
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	L = inStream.getSysSize()

	width = []
	stress = []
	strength = []
	
	while inStream.hasData:
		data = inStream.getSimData()			# Get data for one simulation
		if len(data) > 0:
			# Find the position of the maximum current
			maxCurr = max(data['failCurr'])
			ind = list(data['failCurr']).index(maxCurr)
			# Find clusters till max current
			clusters = dia1P_dualClusters(data['node1'][0:ind+1]-1,data['node2'][0:ind+1]-1,L)

			# Find the widest cluster
			#maxWidth, widestClus = dia1P_largestCluster(clusters,L,'W')

			# Find the cluster that the critical fuse belongs to 
			site = dia1P_nodeToDualLattice(data['node1'][ind]-1,data['node2'][ind]-1,L)
			for cluster in clusters:
				if site in cluster:
					maxWidth = dia1P_dualClusterSize(cluster,L,'W')-1

			width.append(maxWidth)
			stress.append(maxCurr)
			#strength.append(data['fuseStrength'][ind])
		
	#return numpy.array(width), numpy.array(stress), numpy.array(strength)
	return numpy.array(width), numpy.array(stress)

def dia1P_criticalDualClusterGeometry(fName,statType):
	"""
	Returns the heights, widths, sizes and aspects of the critical clusters
	statType: 
	S = choose the largest cluster at critical current
	W = choose the widest cluster at critical current
	H = choose the 'highest' cluster at critical current
	A = choose the cluster with the largest aspect at critical current
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	L = inStream.getSysSize()

	heights = []
	widths = []
	sizes = []
	aspects = []
	
	while inStream.hasData:
		data = inStream.getSimData()
		if len(data) > 0:
			# Find the position of the maximum current
			maxCurr = max(data['failCurr'])
			ind = list(data['failCurr']).index(maxCurr)
			# Find clusters till max current
			clusters = dia1P_dualClusters(data['node1'][0:ind+1]-1,data['node2'][0:ind+1]-1,L)
			maxSize, largestClus = dia1P_largestDualCluster(clusters,L,statType)						# Find "largest" cluster

			# Note stats of the "largest" cluster	
			heights.append(dia1P_dualClusterSize(largestClus,L,'H'))
			widths.append(dia1P_dualClusterSize(largestClus,L,'W'))
			sizes.append(dia1P_dualClusterSize(largestClus,L,'S'))
			aspects.append(dia1P_dualClusterSize(largestClus,L,'A'))
	
	stats = dict()
	stats['heights'] = heights
	stats['widths']		= widths
	stats['sizes']		= sizes
	stats['aspects']		= aspects
		
	return stats

################## DRAWING FUNCTIONS #####################

def dia1P_drawDualClusters(clusters,L,color='random',edgecolor='none',text='off'):
	"""
	Plots the clusters given in the set clusters.
	color:
		random = each cluster in a random color
		other = all clusters in the given color
	edgecolor:
		none = no edgecolor for individual fuses
		other = given edgecolor
	text:
		off = no text
		on =  each fuse has annotation 
	"""
	fig = plt.figure(figsize=(8,8))
	ax  = fig.add_axes([0,0,1,1])
	unit = 0.98/L				# Length unit for the figure. 

	patches = []
	colors = []
	cNum = 0
	for cluster in clusters:
		clusCol = 100*numpy.random.rand()
		cNum = cNum + 1
		for site in cluster:
			corner = (site[0]*unit+0.01,site[1]*unit+0.01)		# Corner of the rectangle.
			center = (site[0]*unit+0.01+unit/2,site[1]*unit+0.01+unit/2)		# Center of the rectangle.
			if text == 'on':
				plt.text(center[0],center[1], str(cNum) + ": " + str(site), ha = "center",family="sans-serif", size=14)
			if color == 'random':
				art = mpl.patches.Rectangle(corner, unit, unit,ec="none")
				colors.append(clusCol)
			else:
				art = mpl.patches.Rectangle(corner, unit, unit,ec="none")
			patches.append(art)

	
	if color == 'random':
		collection = mpl.collections.PatchCollection(patches,cmap=mpl.cm.jet, alpha=0.6)
		#collection = mpl.collections.PatchCollection(patches)
		collection.set_array(numpy.array(colors))
	else:
		collection = mpl.collections.PatchCollection(patches,cmap=mpl.cm.jet, alpha=0.6,facecolor=color)
		#collection = mpl.collections.PatchCollection(patches,facecolor=color)


	collection.set_edgecolor(edgecolor)
	ax.add_collection(collection)

	ax.set_xticks([])
	ax.set_yticks([])

	plt.show()
	return fig

def dia1P_drawDualCriticalClusters(fName,color='random'):
	"""
	Plots the configuration at critical current of the first simulation 
	in the file fName.
	color:
		random: Each cluster in a different color
		other : All clusters in the given color
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	data = inStream.getSimData()

	L = inStream.getSysSize()

	# Find the position of the maximum current
	maxCurr = max(data['failCurr'])
	ind = list(data['failCurr']).index(maxCurr)
	# Find clusters till max current
	clusters = dia1P_dualClusters(data['node1'][0:ind+1]-1,data['node2'][0:ind+1]-1,L)

	return dia1P_drawDualClusters(clusters,L,color=color)

def dia1P_drawLargestCriticalDualCluster(fName,color='random',statType='S'):
	"""
	Plots the largest cluster at critical current.
	color:
		random: random color
		other : given color
	statType:
		S: Largest by size 
		W: Largest by width
		H: Largest by height
		A: Largest by aspect
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	data = inStream.getSimData()

	L = inStream.getSysSize()

	# Find the position of the maximum current
	maxCurr = max(data['failCurr'])
	ind = list(data['failCurr']).index(maxCurr)
	# Find clusters till max current
	clusters = dia1P_dualClusters(data['node1'][0:ind+1]-1,data['node2'][0:ind+1]-1,L)

	# Find the largest cluster
	maxSize, largestClus = dia1P_largestDualCluster(clusters,L,statType)

	# Create a list of clusters ...
	clusters = []
	clusters.append(largestClus)

	return dia1P_drawDualClusters(clusters,L,color=color)

def dia1P_drawAllDualClusters(fName,color='random'):
	"""
	Plots the final configuration of the first simulation 
	in the file fName.
	color:
		random: Each cluster in a different color
		other : All clusters in the given color
	"""
	## Get data from the input file.
	inStream = dia1P_dataStream(fName)
	data = inStream.getSimData()

	L = inStream.getSysSize()

	clusters = dia1P_dualClusters(data['node1']-1,data['node2']-1,L)

	return dia1P_drawDualClusters(clusters,L,color=color)

################ CLUSTERING ALGORITHM ####################
def dia1P_dualClusters(node1,node2,L):
	"""
	Returns a set of clusters on the dual lattice for the nodes in node1 and node2. 
	The nodes are expected to be "zero indexed" meaning that 
	the node in the bottom left corner must be (0,L/2)
	"""
	clusters = []				# list of clusters.
	visited = set()			# Nodes that have already been visited 
	toVisit = set()			# Set of nodes to visit.
	
	if len(node1) > 0 and len(node1) == len(node2):
		# Populate the set of all nodes that need to be visited
		for j in range(len(node1)):
			toVisit.add(dia1P_nodeToDualLattice(node1[j],node2[j],L))	

		## Todo: This piece of code does not seem to be well written.. 
		while len(toVisit) > 0:
			site = toVisit.pop()		# This is the beginning of a new cluster
			toVisit.add(site)
			cl = set()							# Create and initialize the new cluster	
			cl.add(site)
			candidates = set()
			candidates.add(site)
			while len(candidates) > 0:
				site = candidates.pop()
				cl.add(site)
				toVisit.remove(site)
				visited.add(site)
				ngbs = dia1P_dualLatticeNgbs(site[0],site[1],L) - visited		# Get a set of all neighbours of the site that have not been visited
				ngbs = ngbs & toVisit			# New cluster members
				candidates.update(ngbs)

			clusters.append(cl)

	return clusters

def dia1P_orgClusters(node1,node2,L):
	"""
	Returns a set of clusters on the original lattice for the nodes in node1 and node2. 
	The nodes are expected to be "zero indexed" meaning that 
	the node in the bottom left corner must be (0,L/2)
	"""
	clusters = []				# list of clusters.
	visited = set()			# Nodes that have already been visited 
	toVisit = set()			# Set of nodes to visit.
	
	if len(node1) > 0 and len(node1) == len(node2):
		# Populate the set of all nodes that need to be visited
		for j in range(len(node1)):
			toVisit.add((node1[j],node2[j]))	

		## Todo: This piece of code does not seem to be well written.. 
		while len(toVisit) > 0:
			site = toVisit.pop()		# This is the beginning of a new cluster
			toVisit.add(site)
			cl = set()							# Create and initialize the new cluster	
			cl.add(site)
			candidates = set()
			candidates.add(site)
			while len(candidates) > 0:
				site = candidates.pop()
				cl.add(site)
				toVisit.remove(site)
				visited.add(site)
				ngbs = dia1P_orgLatticeNgbs(site[0],site[1],L) - visited		# Get a set of all neighbours of the site that have not been visited
				ngbs = ngbs & toVisit			# New cluster members
				candidates.update(ngbs)

			clusters.append(cl)

	return clusters

def dia1P_orgClustersS(nodeS,L):
	"""
	Same as dia1P_orgClusters(); The only difference being that the nodes are input as
	a set instead of list
	"""
	n1 = []
	n2 = []
	for nd in nodeS:
		n1.append(nd[0])
		n2.append(nd[1])

	return dia1P_orgClusters(n1,n2,L)

################# CLUSTER ENSAMBLE GENERATION ############
def dia1P_generateEnsamble(L,dilution,nSamp,distType,distPar,fNameBase):
	"""
	Generates nSamp samples of LxL lattice with given dilution. The 
	cluster sizes are generated according to the given distType:
	distType = 'P' : power law with exponent distPar
	distType = 'E' : exponential with exponent distPar
	
	The nodes are written to the file fNameBase.node
	The cluster size distribution is written to the file fNameBase.clusSize
	The cluster width distribution is written to the file fNameBase.clusWidth
	"""

	# Open files for writing
	nodeFile = open(fNameBase+'.node','w+')
	clusFile = open(fNameBase+'.clusSize','w+')
	widthFile = open(fNameBase+'.clusWidth','w+')

	# lists for storing the size and width distributions
	sizes = []
	widths = []

	nClus = 0		# Total number of clusters

	for sNum in range(nSamp):		# Working on the sNum^th instance
		# Generate cluster sizes
		cSizes = []
		if distType == 'P':	
			# Find the number of variables to draw
			nVar = pylab.floor(L*L*dilution*scipy.special.zeta(distPar,1)/scipy.special.zeta(distPar-1,1))
			cSizes = list(statUtils.discretePow(int(nVar),t=distPar).astype(int))
		elif distType == 'E':
			nVar = pylab.floor(L*L*dilution*(1 - pylab.exp(-distPar)))
			cSizes = list(statUtils.discreteExpn(int(nVar),l=distPar).astype(int))

		# Generate clusters
		clusters, n1, n2 = dia1P_generateDualClusters(L,cSizes)	
		# Write clusters to the file
		dia1P_writeClus(nodeFile,L,clusters)

		nClus = len(clusters)		# Update the total number of clusters

		# Update size and width distributions
		for clus in clusters:
			sz = dia1P_dualClusterSize(clus,L,statType='S')							# Calculate the size of the cluster
			wd = dia1P_dualClusterSize(clus,L,statType='W')							# Calculate the width of the cluster
			sizes.append(sz)
			widths.append(wd)
	
	# Write distributions to files
	for size in sizes:
		clusFile.write(str(size) + '\n')

	for width in widths:
		widthFile.write(str(width) +'\n')

	# Close files
	nodeFile.close()
	clusFile.close()
	widthFile.close()
		

################# CLUSTER GENERATION #####################
def dia1P_generateDualClusters(L,clusSize,collisionCheck=True):
	"""
	This function generates clusters on a L size diamond lattice with 1-Periodic boundary conditions.
	The sizes of clusters are as specified in clusSize. The algorithm constructs 
	the jth cluster choosing a site at random and adding neighbours at random till 
	the size of the cluster reaches the size specified in clusSize[j-1].
	If this process results in a "collision" with an existing cluster, 
	then the cluster is discarded, and the process started afresh (just the current cluster,
	not the whole process).
	"""
	occupiedSites  = set()		# Set of all occupied sites (dual lattice)
	availableSites = set()		# Set of all sites available for occupation (dual lattice)

	availableNodes = set()		# Set of all nodes available for occupation (in original lattice)

	clusters = []							# A list of all clusters
	
	# Populate the set of available sites (dual lattice)
	availableSites = set(itr.product(range(L),repeat=2))	
	
	# Populate the set of available nodes (original lattice)
	for site in availableSites:
		availableNodes.add(dia1P_dualLatticeToNode(site[0],site[1],L))

	# Create the larger clusters first to avoid cluttering.
	clusSize.sort()
	clusSize = clusSize[::-1]
	
	clus_j = 0								# We are working on creating the jth cluster (size = clusSize[clus_j])
	attempt = 0
	while clus_j < len(clusSize) and attempt < 50:
		if attempt > 40:
			print "Cluster No: " + str(clus_j+1) + " Size: " + str(clusSize[clus_j]) + " Attempt = " + str(attempt)
		size = clusSize[clus_j]			# The size of the cluster to be created.
		clusterNgbs = set()					# Set of all neighbors of the cluster
		cluster = set()							# Set of all sites in the cluster
		clusterNodes = set()				# The cluster represented as a set of nodes in the original lattice
		discard = False							# Flag indicating that this cluster needs to be discarded and build again
																# because it has touched another cluster.

		site = (random.sample(availableSites,1))[0]					# Choose an apropriate first site
		ngbs =	dia1P_dualLatticeNgbs(site[0],site[1],L) 		# All neighbours of the site
		cluster.add(site)
		clusterNgbs.update(ngbs)
		clusterNodes.add(dia1P_dualLatticeToNode(site[0],site[1],L))
		# If any of the neighbours of this site is occupied, then choosing this 
		# site will mean additing to a previously existing cluster, which is not allowed.
		discard = not ngbs.isdisjoint(occupiedSites)
		
		while discard == False and len(cluster) < clusSize[clus_j]:
			site = (random.sample(clusterNgbs,1))[0]
			ngbs =	dia1P_dualLatticeNgbs(site[0],site[1],L) 		# All neighbours of the site
			cluster.add(site)
			clusterNgbs.update(ngbs)
			clusterNodes.add(dia1P_dualLatticeToNode(site[0],site[1],L))
			# If any of the neighbours of this site is occupied, then choosing this 
			# site will mean merging with a previously existing cluster, which is not allowed.
			discard = not ngbs.isdisjoint(occupiedSites)
		

		if discard == False:
			clus_j = clus_j + 1
			clusters.append(cluster)
			occupiedSites.update(cluster)
			availableSites = availableSites - cluster
			availableNodes = availableNodes - clusterNodes
			attempt = 0
		else:
			attempt = attempt + 1
		
	
	#dia1P_drawDualClusters(clusters,L,color='k')
	# Check if there are suitable number of suitable sized clusters
	if len(clusters) != len(clusSize):
		print "Number of clusters differ"
	genLen = []
	for clus in clusters:
		genLen.append(len(clus))

	genLen.sort()
	genLen = genLen[::-1]
	if genLen != clusSize:
		print "Cluster sizes differ"

	# Check for holes and plug them
	holes = set()		# Set of all nodes that are "holes"
	nodeClusters = dia1P_orgClustersS(availableNodes,L)	# Cluster of all unoccupied nodes in the lattice
	if len(nodeClusters) != 1:	# There are isolated clusters, some might be holes
		for Clus in nodeClusters:	
			isHole = True
			# See if any node in the cluster touches the boundary, if so, then it is not a hole
			for node in Clus:	
				if min(node) < L/2 or max(node) >= L*L/2:
					isHole = False
			if isHole == True: 	# If this is a hole then add it to the set of holes
				holes.update(Clus)	
				#print "Before Plugging: Hole Found"

		# Create a list of all nodes
		if len(holes) != 0:
			n1 = []
			n2 = []
			for clus in clusters:
				for site in clus:
					node = dia1P_dualLatticeToNode(site[0],site[1],L)
					n1.append(node[0])
					n2.append(node[1])
			for node in holes:
				n1.append(node[0])
				n2.append(node[1])

			clusters = dia1P_dualClusters(n1,n2,L)

	# Check if the cluster has created any disconnected nodes
	"""
	nodeClusters = dia1P_orgClustersS(availableNodes-holes,L)
	if len(nodeClusters) != 1:
		for clus in nodeClusters:
			isHole = True
			for node in clus:
				if min(node) < L/2 or max(node) >= L*L/2:
					isHole = False
			if isHole == True: 	# If this is a hole then add it to the set of holes
				print "Holes Found after plugging"

	if len(clusters) != len(clusSize):
		print "Number of clusters differ after plugging"
	#dia1P_drawDualClusters(clusters,L,color='k')
	"""
	n1 = []
	n2 = []
	for clus in clusters:
		for site in clus:
			node = dia1P_dualLatticeToNode(site[0],site[1],L)
			n1.append(node[0])
			n2.append(node[1])
	
	return clusters, n1, n2

################# GEOMETRY FUNCTIONS #####################

def dia1P_orgIsDisconnected(node,n1,n2,L):
	"""	
	Returns True if the node "node" on the original lattice is 
	disconnected from the top and bottom buses. n1 and n2 are the 
			ngbsToVisit = intactNodes.intersect(dia1P_orgLatticeNgbs(nextNode[0],nextNode[1],L)) - visited
	path from node to either of the buses. It is assumed that
	the node "node" is intact (otherwise the result is trivially False)
	"""	
	retVal = True	# Default return value
	toVisit = set() # Set of all nodes to be visited.
	visited = set()	# Set of all nodes that have been visited.
	intactNodes = set()	# Set of all intactNodes

	# Populated intactNodes
	intactNodes.add(node)
	for k in range(len(n1)):
		intactNodes.add((n1[k],n2[k]))

	# Initialize toVisit
	toVisit.add(node)

	while len(toVisit) > 0:
		nextNode = toVisit.pop()
		if nextNode[0] < L/2 or nextNode[1] >= L*(L/2):
			retVal = False
			break
		else:
			visited.add(nextNode)
			# Find all neighbors of the current node that are intact and have not been visisted
			ngbsToVisit = intactNodes.intersection(dia1P_orgLatticeNgbs(nextNode[0],nextNode[1],L)) - visited
			# Update the set of nodes to be visited.
			toVisit.update(ngbsToVisit)

	return retVal

def dia1P_orgIsDisconnectedS(node,intactNodes,L):
	"""
	Same as dia1P_orgIsDisconnected(). The intact nodes are 
	input as a set instead of lists.
	"""	
	n1 = []
	n2 = []
	for nd in intactNodes:
		n1.append(nd[0])
		n2.append(nd[1])

	return dia1P_orgIsDisconnected(node,n1,n2,L)

def dia1P_dualClusterSize(cluster,L,statType):
	"""
	statType:
		S: Returns the size of the cluster
		W: Returns the width of the cluster
		H: Returns the height of the cluster
		A: Returns the aspect ratio of the cluster (width/height)
	"""
	retVal = 0
	if statType == 'S':
		retVal = len(cluster)
	elif statType == 'W': 
		xCords = set()				# make a set of all x coordinates in the cluster
		for site in cluster:
			xCords.add(site[0])
		retVal = len(xCords)	# The width is equal to the number of distinct x coordinates in the cluster..
	elif statType == 'H': 
		yCords = set()				# make a set of all y coordinates in the cluster
		for site in cluster:
			yCords.add(site[1])
		retVal = len(yCords)	# The heigth is equal to the number of distinct y coordinates in the cluster..
	elif statType == 'A':
		# Compute aspect as the ratio of width to height.
		retVal = (1.0*dia1P_dualClusterSize(cluster,L,'W'))/(1.0*dia1P_dualClusterSize(cluster,L,'H'))

	return retVal

def dia1P_largestDualCluster(clusters,L,statType):
	"""
	Returns the largest cluster and the size of the 
	largest cluster in clusters. 
	statType:
		S: Largest by size 
		W: Largest by width
		H: Largest by height
		A: Largest by aspect
	"""
	largestClus = iter(clusters).next()
	maxSize = dia1P_dualClusterSize(largestClus,L,statType)
	for cluster in clusters:																				
		clusSize = dia1P_dualClusterSize(cluster,L,statType)
		if clusSize > maxSize:
			largestClus = cluster
			maxSize = clusSize

	return maxSize, largestClus

def dia1P_isRightBoundary(n1,n2,L):
	"""
	Returns true if the node (n1,n2) is on the right boundary.
	"""
	# Make so that n1 is even
	if n1%2 != 0:
		temp = n1
		n1 = n2
		n2 = temp

	if n1%L == 0 and (n2+1)%L ==0:
		return True
	else:
		return False

def dia1P_nodeToDualLattice(n1,n2,L):
	"""
	Given the node numbers n1, n2 and the lattice size L (assumed even)
	this routine returns the (i,j) lattice positions of the 
	bond connecting the nodes. The nodes are assumed be 
	numbered so that (0,N/2) is the first bond on the 
	bottom left. The lattice number corresponding to this 
	bond is (0,0)
	"""
	# Order n1, n2 so that n1 < n2
	if n1 > n2:
		temp = n1
		n1 = n2
		n2 = temp

	# We first find 'i' of the tuple (i,j)
	if dia1P_isRightBoundary(n1,n2,L):		# Deal with the right boundary separately
		i = L - 1
	else:
		a1 = n1%L
		a2 = n2%L

		if a1 < L/2:
			a1 = 2*a1
		else:
			a1 = 2*(a1 - L/2) + 1

		if a2 < L/2:
			a2 = 2*a2
		else:
			a2 = 2*(a2 - L/2) + 1

		i = min(a1,a2)


	# Find j
	j = min(n1/(L/2), n2/(L/2))

	return (i,j)

def dia1P_dualLatticeToNode(i,j,L):
	"""
	Given the coordinates (i,j) on the dual lattice, 
	returns the node numbers of the bonds on the 
	diamond lattice. The lattice number (0,0) corresponds
	to the node (0,L/2).
	"""	
	n1 = 0
	n2 = 0
	if i%2 == 0 and j%2 == 0:			# Right leaning, even row
		n1 = i/2 + (j/2)*L
		n2 = n1 + L/2
	elif i%2 == 1 and j%2 == 1:		# Right leaning, odd row
		n1 = (i-1)/2 + j*(L/2)
		if i != L-1:
			n2 = n1 + L/2 + 1
		else:
			n2 = n1 + 1
	elif	i%2 == 1 and j%2 == 0:	# Left leaning, even row
		n1 = ((i+1)/2)%(L/2) + (j/2)*L
		if i != L-1:
			n2 = n1 + L/2 -1
		else:
			n2 = n1 + L -1
	elif i%2 ==0 and j%2 == 1:		# Left leaning, odd row
		n1 = i/2 + j*(L/2)
		n2 = n1 + L/2

	return (n1, n2)


def dia1P_dualLatticeToBond(i,j,L):
	"""
	Given the lattice position of a bond, return its bond number. 	
	"""
	return j*L + i + 1

def dia1P_dualLatticeNgbs(i,j,L):
	"""
	Returns a set of all neighbours of the lattice point (i,j) 
	in the dual lattice
	"""
	ngbs = set()

	# Upper neighbours
	if j > 0:					# Not on lower boundary
		ngbs.add((i,j-1))

	if j < L-1: 			# Not on upper boundary
		ngbs.add((i,j+1))
	
	# Side neighbours
	ngbs.add(((i-1)%L,j))
	ngbs.add(((i+1)%L,j))


	# Diagonal neighbours
	if j%2 == i%2:		# Right tilt
		if j < L-1:
			ngbs.add(((i-1)%L,j+1))
		if j > 0:
			ngbs.add(((i+1)%L,j-1))
	else: 						# Left tilt
		if j < L-1:
			ngbs.add(((i+1)%L,j+1))
		if j > 0:
			ngbs.add(((i-1)%L,j-1))

	return ngbs

def dia1P_orgLatticeNgbs(n1,n2,L):
	"""
	Returns the set of all neighbors of the node (n1,n2) for a 
	system of size L. The bottom left node is (0,L/2), and n1 < n2.
	"""
	
	if n1 > n2:		# Sort nodes
		print "Unsorted nodes in dia1P_orgLatticeNgbs()"
		temp = n1
		n1 = n2
		n2 = temp

	ngbs = set()				# Set of all neighbors of the present node
	self = set()				# Set representing the present node
	self.add((n1,n2))		

	if n1 < L/2: 				# n1 on bottom row
		ngbs.add((n1,n1+L/2))		# upper right bond
		# upper left bond							
		if n1%L == 0:		
			ngbs.add((n1,L-1))
		else:
			ngbs.add((n1,n1+L/2-1))
	elif n1%L < L/2:		# "even" row
		ngbs.add((n1-L/2,n1))		# lower right
		ngbs.add((n1,n1+L/2))		# upper right 
		if n1%L == 0:	# left boundary
			ngbs.add((n1-1,n1))				# lower left
			ngbs.add((n1,n1+L-1))			# upper left
		else:
			ngbs.add((n1-L/2-1,n1))		# lower left
			ngbs.add((n1,n1+L/2-1))		# upper left
	else:
		ngbs.add((n1-L/2,n1))				# lower left
		ngbs.add((n1,n1+L/2))				# upper left
		if (n1+1)%L == 0:		# right boundary
			ngbs.add((n1,n1+1))				# upper right
			ngbs.add((n1-L+1,n1))			# lower right
		else:
			ngbs.add((n1-L/2+1,n1))		# upper right
			ngbs.add((n1,n1+L/2+1))		# lower right
	

	if n2 >= (L/2)*L:	# n1 on upper row
		ngbs.add((n2-L/2,n2))		# lower right bond
		# lower left bond							
		if n2%L == 0:		
			ngbs.add(((L/2)*L-1,n2))
		else:
			ngbs.add((n2-L/2-1,n2))
	elif n2%L < L/2:		# "even" row
		ngbs.add((n2-L/2,n2))		# lower right
		ngbs.add((n2,n2+L/2))		# upper right 
		if n2%L == 0:	# left boundary
			ngbs.add((n2-1,n2))				# lower left
			ngbs.add((n2,n2+L-1))			# upper left
		else:
			ngbs.add((n2-L/2-1,n2))		# lower left
			ngbs.add((n2,n2+L/2-1))		# upper left
	else:
		ngbs.add((n2-L/2,n2))				# lower left
		ngbs.add((n2,n2+L/2))				# upper left
		if (n2+1)%L == 0:		# right boundary
			ngbs.add((n2,n2+1))				# upper right
			ngbs.add((n2-L+1,n2))			# lower right
		else:
			ngbs.add((n2-L/2+1,n2))		# upper right
			ngbs.add((n2,n2+L/2+1))		# lower right
	
		
	ngbs = ngbs - self

	return ngbs


#################### TESTING ##########################

def dia1P_testCases():
	"""
	Uses various tests to find if various functions
	are bug free. 
	"""
	L = 6
	
	n1 = [0 , 1, 1, 2, 2, 0, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 6, 9, 9,10,10,11,11,12,13,13,14,14,12,15,15,16,16,17,17]
	n2 = [3 , 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 6, 9, 9,10,10,11,11,12,13,13,14,14,12,15,15,16,16,17,17,18,19,19,20,20,18]
	i =  [0 , 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5]
	j =  [0	, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5]
	bondNum = pylab.arange(L*L) + 1

	print "Testing dia1P_dualLatticeToNode(i,j,L) with L = 6"
	for k in range(len(i)):
		site = dia1P_dualLatticeToNode(i[k],j[k],L)
		if site[0] == n1[k] and site[1] == n2[k]:
			print "Test passed for (i,j) = (" + str(i[k]) + "," + str(j[k]) + "). Result: (n1,n2) = " + str(site)
		else:
			print "Test failed for (i,j) = (" + str(i[k]) + "," + str(j[k]) + "). Result: (n1,n2) = " + str(site)
			return

	print "Testing dia1P_dualLatticeToNode(i,j,L) and dia1P_nodeToDualLattice(n1,n2,L) recursively for L = 10"
	for l1 in range(10):
		for l2 in range(10):
			node = dia1P_dualLatticeToNode(l1,l2,10)
			site = dia1P_nodeToDualLattice(node[0],node[1],10)
			if site[0] == l1 and site[1] == l2:
				print "Test passed for (i,j) = (" + str(l1) + "," + str(l2) + "). Result: (n1,n2) = " + str(node)
			else:
				print "Test failed for (i,j) = (" + str(l1) + "," + str(l2) + "). Result: (n1,n2) = " + str(node)
				return

	print "Testing dia1P_isRightBoundary(n1,n2,L) with L = 6"
	for k in range(len(n1)):
		bdr = dia1P_isRightBoundary(n1[k],n2[k],L)
		if bdr and i[k] != 5:
			print "Test failed for (n1,n2) = (" + str(n1[k]) + "," + str(n2[k]) + "). Result: " + str(bdr)
			return
		else:
			print "Test passed for (n1,n2) = (" + str(n1[k]) + "," + str(n2[k]) + "). Result: " + str(bdr)


	print "Testing dia1P_nodeToDualLattice(n1,n2,L) with L = 6"
	for k in range(len(n1)):
		site = dia1P_nodeToDualLattice(n1[k],n2[k],L)
		if site[0] == i[k] and site[1] == j[k]:
			print "Test passed for (n1,n2) = (" + str(n1[k]) + "," + str(n2[k]) + "). Result: (i,j) = " + str(site)
		else:
			print "Test Failed for (n1,n2) = (" + str(n1[k]) + "," + str(n2[k]) + "). Result: (i,j) = " + str(site)
			return

	print "Testing dia1P_dualLatticeToBond(i,j,L) with L = 6"
	for k in range(len(i)):
		if bondNum[k] == dia1P_dualLatticeToBond(i[k],j[k],L):
			print "Test passed for (i,j) = (" + str(i[k]) + "," + str(j[k]) + "). Result: bn = " + str(dia1P_dualLatticeToBond(i[k],j[k],L))
		else:
			print "Test failed for (i,j) = (" + str(i[k]) + "," + str(j[k]) + "). Result: bn = " + str(dia1P_dualLatticeToBond(i[k],j[k],L))
			return

	print "Testing dia1P_dualLatticeNgbs(i,j,L) with L = 6"
	i = 0
	j = 0
	ngbs = set([	(1 ,0 ), (0 ,1 ),	(5 ,0 ), (5 ,1 ) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 0
	j = 5
	ngbs = set([	(5 ,5 ), (1 ,5 ),	(0 ,4 ), (5 ,4 ) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 5
	j = 5
	ngbs = set([	(0 ,5 ), (5 ,4 ),	(0 ,4 ), (4 ,5 ) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 5
	j = 0
	ngbs = set([	(0 ,0 ), (5 ,1 ),	(0 ,1 ), (4 ,0 ) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 0
	j = 4
	ngbs = set([	(0 ,5 ), (0 ,3 ),	(1 ,4 ), (5 ,4 ), (5 ,5), (1 ,3) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 0
	j = 3
	ngbs = set([	(0 ,4 ), (0 ,2 ),	(1 ,3 ), (5 ,3 ), (5 ,2), (1 ,4) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 1
	j = 1
	ngbs = set([	(2 ,1 ), (0 ,1 ),	(1 ,2 ), (1 ,0 ), (0 ,2), (2 ,0) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

	i = 2
	j = 3
	ngbs = set([	(1 ,3 ), (3 ,3 ),	(2 ,4 ), (2 ,2 ), (3 ,4), (1 ,2) ])
	calNgbs = dia1P_dualLatticeNgbs(i,j,L)
	if ngbs == calNgbs:
		print "Test passed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
	else:
		print "Test failed for (i,j) = (" + str(i) + "," + str(j) + "). Result: ngbs = " + str(calNgbs)
		return

