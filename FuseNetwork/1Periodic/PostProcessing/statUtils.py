import pylab
import numpy
import matplotlib as mpl
import scipy
import scipy.optimize as sop
import scipy.special
import random

def isSorted(a):
	"""
	Returns true if the indexable object a is sorted.
	"""
	for j in range(len(a) - 1):
		if a[j] > a[j+1]:
			return False
	return True

def discreteExpn(N,l = pylab.log(2)):
	"""
	Returns N random posetive integers sampled from a discrete 
	exponential distribution. The density mass function is given by
	F(n) = A*Exp(-l*n), where A = Exp(l) - 1.
	The output is sorted.
	"""
	unif = numpy.random.uniform(size = N)		# Draw N random variables from a uniform distribution
	unif.sort()
	samp = numpy.zeros(N)										# samp will hold the discrete exponential random variables. 
	A = pylab.exp(l) - 1

	guess = 1		# This is a guess to the discrete exponential
	cummProb = A*pylab.exp(-l*1.0) 
	for j in range(N):											
		while unif[j] > cummProb:
			guess = guess+1
			cummProb = cummProb + A*pylab.exp(-l*guess)

		samp[j] = guess

	return samp

def discretePow(N,t = 3):
	"""
	Returns N random posetive integers sampled from a discrete 
	power law distribution. The density mass function is given by
	F(n) = n^(-t)/W
	The output is sorted.
	"""
	
	unif = numpy.random.uniform(size = N)		# Draw N random variables from a uniform distribution
	unif.sort()
	samp = numpy.zeros(N)										# samp will hold the discrete power law variables. 

	guess = 1.0		# This is a guess to the discrete power law variable
	W = scipy.special.zeta(t,1)
	cummProb = 1.0/W
	for j in range(N):
		#print "j = " + str(j) + " of " + str(N)											
		while unif[j] > cummProb:
			#print "Unif = " + str(unif[j]) + " guess = " + str(guess) + " cummProb = " + str(cummProb)
			guess = guess+1
			cummProb = cummProb + (guess**(-t))/W

		#print "Unif = " + str(unif[j]) + " CummProb = " + str(cummProb)
		#print str(j+1) + " random number = " + str(guess)	
		#foo = input('Move?')
		samp[j] = guess

	return samp

def discreteRawPDF(data):
	"""
	Returns the raw (unbinned) PDF for the discrete data in 'data'.
	"""
	pdf = dict()
	support = numpy.array(pylab.unique(data))
	support.sort()
	pSupport = numpy.zeros(len(support))
	
	for s in support:
		pdf[s] = 0.0

	for d in data:
		pdf[d] = pdf[d] + 1.0

	for j in range(len(support)):
		pSupport[j] = pdf[support[j]]

	pSupport = pSupport/sum(pSupport)
	
	return support, pSupport

def empCDF(data, X):
	"""
	Returns the emperical CDF for the data given in the array d 
	evaluated at the points in X. 
	"""
	if isSorted(data) and isSorted(X):
		n = len(data)
		cdf = numpy.zeros(len(X))
	
		c = 1.0					# A running variable that stores the current value of the cdf
		inc = 1.0/n			# At each data point the cdf jumps by inc
		k = n-1					# The index of the array 'data' that corresponds to a value in X

		# Construct an array that runs [N-1, N-2, ... 0] where N = len(X)
		itr = range(len(X))
		itr = itr[::-1]

		for j in itr:
			while(data[k] > X[j] and k > 0):
				c = c - inc
				k = k -1
			cdf[j] = c
			
		return cdf
	else:
		print "empCDF expects sorted inputs"
		return []

def lsqReg(X,Y):
	"""
	Returns the least square fit of Y = a*X + b. 
	"""	
	m_x = pylab.mean(X)
	m_y = pylab.mean(Y)
	m_x2 = pylab.mean(X*X)
	m_xy = pylab.mean(X*Y)

	a = (m_xy - m_x*m_y)/(m_x2 - m_x*m_x)
	b = m_y - a*m_x

	return a,b

def emperical_gumbel_cfun(p,d,sp,L):
	"""
	Returns the cost function for approach of the emperical survival distribution
	function to the gumbel.
	sum ( G((d - B)/A) - sp)**2 / N
	"""
	A = p[0]
	B = p[1]
	n = len(d)
	s_norm = (d-B)/A
	G = pylab.exp(-pylab.exp((d-B)/A))

	# Calculate the normalized emperical deviation
	dev_emp = (sp - G)*pylab.log(L*L)
	# Calculate the theoretical deviation
	dev_thr = (3.0/4)*G*pylab.exp(s_norm)*(s_norm**2.0)


	#cost = pylab.sqrt(sum((G-sp)*(G-sp)))/n
	cost = pylab.sqrt(sum((dev_emp - dev_thr)*(dev_emp-dev_thr)))/n
	cost = pylab.sqrt(sum((G-sp)*(G-sp)))/n
	return cost

def emperical_params(p,d,sp,L):
	"""
	Returns the best fit parameters A and B such that 
	sum ( G((d - B)/A) - sp)**2 / N is minimized.
	"""
	# Use the provided estimates as initial guess for the minimizer
	xopt = sop.fmin(emperical_gumbel_cfun,p,args=(d,sp,L),full_output=False,disp=False)

	A = xopt[0]
	B = xopt[1]

	#print "LSQ: (k,l) = (" + str(k_lsq) + " , " + str(l_lsq) + ")"
	#print "MLE: (k,l) = (" + str(k_mle) + " , " + str(l_mle) + ")"
	return A, B

def weibull_mle_cfun(p,data):
	"""
	Returns the weibull mle log cost function (normalized)
	The weibull CDF is 1 - exp(-(x/l)^k).
	"""
	k = p[0]
	l = p[1]
	n = len(data)
	cost = 0
	cost = cost + pylab.log(k/l)
	cost = cost + (k-1)*pylab.sum(pylab.log(data/l))/n
	cost = cost - pylab.sum( (data/l)**k)/n

	return -cost

def weibull_lsq(data):
	"""
	Returns the weibull parameters estimated by using 
	the least square method for the given data.
	The weibull CDF is 1 - exp(-(x/l)^k).
	One should be aware of the fact that this approach weighs 
	the extreme (small or large) observations more than the 
	bulk.
	"""

	# Evaluate the emperical CDF at the observations
	# and rescale to convert into emperical probability
	n = len(data)
	print type(data)
	print type(empCDF(data,data))	
	ecdf = empCDF(data,data)*n/(1.0  + n)	

	# Make the array of "infered" variables and independent variables
	y = pylab.log(-pylab.log(1-ecdf))
	x = pylab.log(data)

	# estimate regression coefficients of y = a*x + b
	a, b = lsqReg(x,y)

	# Extract the weibull parameters
	k = a 
	l = pylab.exp(-b/k)

	return k, l
	
def weibull_mle(data):
	"""
	Returns the mle estimate of weibull parameters.
	The weibull CDF is 1 - exp(-(x/l)^k).
	"""
	# Estimate the parameters by using least squares estimation.
	k_lsq,l_lsq = weibull_lsq(data)

	# Use the LSQ estimate as initial guess for MLE
	xopt = sop.fmin(weibull_mle_cfun,numpy.array([k_lsq,l_lsq]),args=(data,),full_output=False,disp=False)

	k_mle = xopt[0]
	l_mle = xopt[1]

	#print "LSQ: (k,l) = (" + str(k_lsq) + " , " + str(l_lsq) + ")"
	#print "MLE: (k,l) = (" + str(k_mle) + " , " + str(l_mle) + ")"
	return k_mle, l_mle
	
def weibull_moments(k,l):
	"""
	Returns the mean and the standard deviation of the weibull distribution with parameters k,l
	The weibull CDF is 1 - exp(-(x/l)^k).
	"""
	wMean = l*scipy.special.gamma(1 + 1.0/k)
	wVar = l*l*scipy.special.gamma(1 + 2.0/k) - wMean*wMean
	wStdEvp = wVar**0.5

	return wMean, wStdEvp

def weibull_momentCost(x,expMean,expDev):
	wMean, wStdEvp = weibull_moments(x[0],x[1])
	return numpy.array([(expMean - wMean)**2, (expDev - wStdEvp)**2])

def weibull_parFind(wMean,wStdEvp):
	"""
	Returns the weibull parameters that result in the given mean and deviation
	"""
	X = sop.fsolve(weibull_momentCost,numpy.array([wMean,wMean]),args=(wMean,wStdEvp))
	return X

def weibull_cdf(x,k,l):
	"""
	Returns the weibull cdf evaluated at x.
	The weibull CDF is 1 - exp(-(x/l)^k).
	"""
	return 1.0 - pylab.exp( - ((1.0*x/l)**k))

def weibull_icdf(x,k,l):
	"""
	Returns the inverse weibull cdf evaluated at x.
	The weibull CDF is 1 - exp(-(x/l)^k).
	"""
	return l*( (-pylab.log(1.0-x))**(1.0/k))

def gumbel_mle_cfun(p,data):
	"""
	Returns the gumbel mle log cost function (normalized)
	The gumbel CDF is exp( -exp( - (x - m)/bt ) ).
	"""
	m = p[0]
	bt = p[1]
	n = len(data)
	cost = 0
	cost = cost - pylab.log(bt)
	cost = cost - pylab.sum( (data - m)/bt)/n
	cost = cost - pylab.sum( pylab.exp( -(data - m)/bt))/n

	return -cost

def gumbel_lsq(data):
	"""
	Returns the gumbel parameters estimated by using 
	the least square method for the given data.
	The gumbel CDF is exp( -exp( - (x - m)/bt ) ).
	"""

	# Evaluate the emperical CDF at the observations
	# and rescale to convert into emperical probability
	n = len(data)
	ecdf = empCDF(data,data)*n/(1.0  + n)	

	# Make the array of "infered" variables and independent variables
	y = -pylab.log(-pylab.log(ecdf))
	x = data

	# estimate regression coefficients of y = a*x + b
	a, b = lsqReg(x,y)

	# Extract the weibull parameters
	bt = 1.0/a
	m = -b*bt

	return m, bt
	
def gumbel_mle(data):
	"""
	Returns the mle estimate of gubel parameters.
	The gumbel CDF is exp( -exp( - (x - m)/bt ) ).
	"""
	# Estimate the parameters by using least squares estimation.
	m_lsq,bt_lsq = gumbel_lsq(data)

	# Use the LSQ estimate as initial guess for MLE
	xopt = sop.fmin(gumbel_mle_cfun,numpy.array([m_lsq,bt_lsq]),args=(data,),full_output=False,disp=False)

	m_mle = xopt[0]
	bt_mle = xopt[1]

	#print "LSQ: (m,bt) = (" + str(m_lsq) + " , " + str(bt_lsq) + ")"
	#print "MLE: (m,bt) = (" + str(m_mle) + " , " + str(bt_mle) + ")"
	return m_mle, bt_mle
	
def gumbel_cdf(x,m,bt):
	"""
	Returns the gumbel cdf evaluated at x.
	The gumbel CDF is exp( -exp( - (x - m)/bt ) ).
	"""
	return pylab.exp( -pylab.exp( - (x - m)/bt))

def gumbel_icdf(x,m,bt):
	"""
	Returns the inverse gumbel cdf evaluated at x.
	The gumbel CDF is exp( -exp( - (x - m)/bt ) ).
	"""
	return m - bt*pylab.log(-pylab.log(x))


def mingumbel_mle_cfun(p,data):
	"""
	Returns the mingumbel mle log cost function (normalized)
	The gumbel CDF is 1 - exp( -exp( (x - m)/bt ) ).
	"""
	m = p[0]
	bt = p[1]
	n = len(data)
	cost = 0
	cost = cost - pylab.log(bt)
	cost = cost + pylab.sum( (data - m)/bt)/n
	cost = cost - pylab.sum( pylab.exp( (data - m)/bt))/n

	return -cost

def mingumbel_lsq(data):
	"""
	Returns the mingumbel parameters estimated by using 
	the least square method for the given data.
	The gumbel CDF is 1 - exp( -exp( (x - m)/bt ) ).
	"""

	# Evaluate the emperical CDF at the observations
	# and rescale to convert into emperical probability
	n = len(data)
	ecdf = empCDF(data,data)*n/(1.0  + n)	

	# Make the array of "infered" variables and independent variables
	y = pylab.log(-pylab.log(1-ecdf))
	x = data

	# estimate regression coefficients of y = a*x + b
	a, b = lsqReg(x,y)

	# Extract the weibull parameters
	bt = 1.0/a
	m = -b*bt

	return m, bt
	
def mingumbel_mle(data):
	"""
	Returns the mle estimate of mingumbel parameters.
	The gumbel CDF is 1 - exp( -exp( (x - m)/bt ) ).
	"""
	# Estimate the parameters by using least squares estimation.
	m_lsq,bt_lsq = mingumbel_lsq(data)

	# Use the LSQ estimate as initial guess for MLE
	xopt = sop.fmin(mingumbel_mle_cfun,numpy.array([m_lsq,bt_lsq]),args=(data,),full_output=False,disp=False)

	m_mle = xopt[0]
	bt_mle = xopt[1]

	#print "LSQ: (m,bt) = (" + str(m_lsq) + " , " + str(bt_lsq) + ")"
	#print "MLE: (m,bt) = (" + str(m_mle) + " , " + str(bt_mle) + ")"
	return m_mle, bt_mle
	
def mingumbel_cdf(x,m,bt):
	"""
	Returns the gumbel cdf evaluated at x.
	The gumbel CDF is 1 - exp( -exp( (x - m)/bt ) ).
	"""
	return 1 - pylab.exp( -pylab.exp( (x - m)/bt))

def mingumbel_icdf(x,m,bt):
	"""
	Returns the inverse gumbel cdf evaluated at x.
	The gumbel CDF is 1 - exp( -exp( (x - m)/bt ) ).
	"""
	return m + bt*pylab.log(-pylab.log(1-x))


def duxbury_mle_cfun(p,L,data):
	"""
	Returns the duxbury mle log cost function (normalized)
	The duxbury CDF is 1 - exp( -(L^2)*exp( - (s/x)^2 ) )
	"""
	s = p[0]
	n = len(data)
	cost = 0
	cost = cost + 2*pylab.log(s)
	cost = cost - 3*pylab.sum( pylab.log(data) )/n
	cost = cost - L*L*pylab.sum( pylab.exp(-((s/data)**2)))/n
	cost = cost - pylab.sum( (s/data)**2)/n

	return -cost

def duxbury_lsq(data,L):
	"""
	Returns the duxbury parameter estimated by using 
	the least square method for the given data.
	The duxbury CDF is 1 - exp( -(L^2)*exp( - (s/x)^2 ) )
	"""

	# Evaluate the emperical CDF at the observations
	# and rescale to convert into emperical probability
	n = len(data)
	ecdf = empCDF(data,data)*n/(1.0  + n)	

	# Make the array of "infered" variables and independent variables
	y = pylab.log(-pylab.log(-pylab.log(1-ecdf)/(L*L) ))
	x = pylab.log(data)

	# estimate regression coefficients of y = a*x + b
	a, b = lsqReg(x,y)

	# Extract the duxbury parameter
	s = pylab.exp(b/2)

	return s
	
def duxbury_mle(data,L):
	"""
	Returns the mle estimate of duxbury parameters.
	The duxbury CDF is 1 - exp( -(L^2)*exp( - (s/x)^2 ) )
	"""
	# Estimate the parameters by using least squares estimation.
	s_lsq = duxbury_lsq(data,L)

	# Use the LSQ estimate as initial guess for MLE
	xopt = sop.fmin(duxbury_mle_cfun,numpy.array([s_lsq]),args=(L,data,),full_output=False,disp=False)

	s_mle = xopt[0]

	#print "LSQ: (s) = (" + str(s_lsq) + ")"
	#print "MLE: (s) = (" + str(s_mle) + ")"
	return s_mle

def duxbury_cdf(X,L,s):
	"""
	Returns the duxbury cdf evaluated at X.
	The duxbury CDF is 1 - exp( -(L^2)*exp( - (s/x)^2 ) )
	"""
	
	return 1 - pylab.exp( -L*L*pylab.exp( -((s/X)**2.0) )) 

def duxbury_icdf(X,L,s):
	"""
	Returns the inverse duxbury cdf evaluated at X.
	The duxbury CDF is 1 - exp( -(L^2)*exp( - (s/x)^2 ) )
	"""

	return (-s*s/pylab.log( -pylab.log(1-X)/(L*L)) )**(0.5)

def fitQuality(L,N):
	"""
	"""	
	# Load raw data
	pth = '/home/ashivni/Work/SVN_WORK/repos/Fracture/Duxbury/trunk/Convergence/DATA/NoShift/'
	fName = 'FAIL_STRESS_DIA_'+str(L/2)+'x'+str(L)+'.dat'
	d = numpy.array(pylab.loadtxt(pth+fName))
	d.sort()
	
	# Find emperical cdf and scale to avoid zeros in logs
	cf = empCDF(d,d)*len(d)/(1.0 + len(d))	
	#cf = empCDF(d,d)

	# Draw a random sample from the data
	samp = numpy.array(random.sample(d,N))
	samp.sort()

	# Find weibull, gumbel and duxbury fit parameters
	wbl_k, wbl_l = weibull_mle(samp)
	gbl_m, gbl_bt = mingumbel_mle(samp)
	dux_s = duxbury_mle(samp,L)

	# Evaluate the various quantiles with the fit parameters
	wICDF = weibull_icdf(cf,wbl_k,wbl_l)
	gICDF = mingumbel_icdf(cf,gbl_m,gbl_bt)
	dICDF = duxbury_icdf(cf,L,dux_s)
	wCDF = weibull_cdf(d,wbl_k,wbl_l)
	gCDF = mingumbel_cdf(d,gbl_m,gbl_bt)
	dCDF = duxbury_cdf(d,L,dux_s)

	fig = mpl.pyplot.figure()
	# Setup for the semilog plot
	fig.subplots_adjust(bottom=0.2,left=0.2)
	ax = fig.add_subplot(111)
	ax.set_xscale('linear')
	ax.set_yscale('log')
	ax.set_xlabel('stress',fontsize=30)
	ax.set_ylabel('P( Failure )',fontsize=30)
	ax.set_xlim([0.20,0.28])
	ax.set_ylim([1E-5,1E-1])
	ax.xaxis.set_ticks([0.20,0.22,0.24,0.26,0.28])
	for t in ax.get_xticklabels():
		t.set_fontsize(20)
	for t in ax.get_yticklabels():
		t.set_fontsize(20)

	"""
	line, = ax.plot(d,wICDF,'g.')
	line, = ax.plot(d,gICDF,'r.')
	line, = ax.plot(d,dICDF,'b.')
	line, = ax.plot(d,d,'k-')
	"""

	line, = ax.plot(d,wCDF,'g-',lw=3,label='Weibull')
	line, = ax.plot(d,gCDF,'r-',lw=3,label='Gumbel')
	line, = ax.plot(d,dCDF,'b-',lw=3,label='Duxbury')
	line, = ax.plot(d,cf,'k.',label='Emperical')

	ax.legend(loc='lower right',fancybox=True)
	leg = ax.get_legend()
	mpl.pyplot.setp(leg.get_texts(),fontsize=30)
	mpl.pyplot.draw()
	mpl.pyplot.savefig('FailureProb.png')

	nTrial = 10
	qRank = 10
	w_est = numpy.zeros(nTrial)
	g_est = numpy.zeros(nTrial)
	d_est = numpy.zeros(nTrial)
	for j in range(nTrial):
		# Draw a random sample from the data
		samp = numpy.array(random.sample(d,N))
		samp.sort()

		# Find weibull, gumbel and duxbury fit parameters
		wbl_k, wbl_l = weibull_mle(samp)
		gbl_m, gbl_bt = mingumbel_mle(samp)
		dux_s = duxbury_mle(samp,L)

		w_est[j] =  weibull_cdf(d[qRank-1],wbl_k,wbl_l) 
		g_est[j] = mingumbel_cdf(d[qRank-1],gbl_m,gbl_bt)
		d_est[j] = duxbury_cdf(d[qRank-1],L,dux_s)
		

	print "Stress = " + str(d[qRank-1]) 
	print "Emperical Failure Probability = " + str(cf[qRank-1]) + " Equiv to 1 in " +  str((1.0/cf[qRank-1]))
	print "Weibull Estimate = " + str(pylab.mean(w_est)) + " +/- " + str(pylab.var(w_est)**0.5)  
	print " Equiv to 1 in " +  str((1/pylab.mean(w_est))) 
	print "Gumbel  Estimate = " + str(pylab.mean(g_est)) + " +/- " + str((pylab.var(g_est)**0.5))
	print " Equiv to 1 in " +  str((1/pylab.mean(g_est))) 
	print "Duxbury Estimate = " + str(pylab.mean(d_est)) + " +/- " + str(pylab.var(d_est)**0.5) 
	print " Equiv to 1 in " +  str((1/pylab.mean(d_est))) 


def logBinInt(base,s,ps):
  # Bin the data in bins with edges base**1, base**2 ... 
  # all s such that base**i < x <= base**(i+1) goes in the bin i+1.
  # It is assumed that all s are strictly posetive integers and 
  # that s is sorted.

  # Check for sorted input
  for ind in range(1,len(s)):
    if s[ind] <= s[ind-1]:
      print "Unsorted input or repeated integers input. Returning."
      return [],[]

  # Construct a list of all edges of the bins
  maxPow = pylab.ceil(pylab.log(max(s))/pylab.log(base)).astype(int)
  maxEdge = pylab.floor(base**maxPow)

  # Generate edges of the bins
  binEdge = [] 
  lastEdge = pylab.floor(base)
  exp = 1.0
  while lastEdge < maxEdge:
    if pylab.floor(base**exp) > lastEdge:
      binEdge.append(lastEdge)
      lastEdge = pylab.floor(base**exp)
    exp = exp+1
  binEdge.append(maxEdge)
  binEdge = numpy.array(binEdge)

  # Calculate bin sizes and centers
  binSize = numpy.zeros(len(binEdge))
  binCenter = numpy.zeros(len(binEdge))
  binSize[0] = binEdge[0]
  binCenter[0] = (1.0 + binEdge[0])*0.5
  for j in range(1,len(binEdge)):
    binSize[j] = binEdge[j] - binEdge[j-1]
    binCenter[j] = (binEdge[j] + binEdge[j-1] + 1)*0.5

  # Calculate bin probability
  binProb = numpy.zeros(len(binEdge))
  binNum = 0
  for data, prob in zip(s,ps):
    while data > binEdge[binNum]:
      binNum = binNum+1
    binProb[binNum] = binProb[binNum] + prob 
  binProb = binProb/binSize

  return binCenter, binProb

