########################################################################
#                                                                      #
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

import numpy as np
import traceback
import pickle
import sys
from math import ceil
try:
    from math import log2
except:
    from math import log
    log2 = lambda x: log(x, 2)

try:
    # Use the pyfftw library if available: Due to the better
    # planning this library will probably yield a better
    # performance than the numpy equivalent. However, using
    # the naive pyfftw.interfaces.numpy_fft.fft() drop-in
    # replacement proved to be detrimental due to the overhead
    # in planning, especially in the case of many small
    # (sub-)domains. Hence, the planner effort is set to
    # minimal and the accumulated Wisdom is cached as much as
    # possible.
    import pyfftw
    import multiprocessing
    cpu_count = multiprocessing.cpu_count()
    align = pyfftw.simd_alignment
    buffers = {}
    solvers = {}
    def fft(*args, **kwargs):
        func, dtype = ('ifft', 'complex64') if kwargs.get('ifft', False) else ('fft', 'float64')
        shape_key = dtype, args[0].shape
        buffer = buffers.get(shape_key) 
        if buffer is None:
            buffer = buffers[shape_key] = pyfftw.n_byte_align_empty(shape_key[1], align, shape_key[0])
        buffer[:] = args[0]
        args2 = [buffer] + list(args[1:])
        solver_key = (shape_key, args[1], kwargs['axis'])
        solver = solvers.get(solver_key)
        if solver is None:
            kwargs.pop('ifft', None)
            solver = solvers[solver_key] = getattr(pyfftw.builders, func)(*args2, **dict(kwargs, threads=cpu_count, planner_effort='FFTW_ESTIMATE'))
        return solver(buffer)
    def ifft(*args, **kwargs):
        return fft(*args, **dict(kwargs, ifft=True))
except:
    fft = np.fft.fft
    ifft = np.fft.ifft

def PML(cnf,PMLcells,rho):
    # PML coefficients in free air
    alphaPMLp = cnf.ampmax*np.power(np.arange(0.5,PMLcells-0.5+1.,1.)/PMLcells,4)  # attenuation coefficients of PMLcells for pressure
    alphaPMLu = rho*cnf.ampmax*np.power(np.arange(0.,PMLcells+1.,1.)/PMLcells,4)   # attenuation coefficients of PMLcells for velocity

    return alphaPMLp, alphaPMLu

def Rmatrices(rho1,rho,rho2):
    Zn1 = rho1/rho
    Rlw1 = (Zn1-1.)/(Zn1+1.)
    Rlw2 = (np.power(Zn1,-1.)-1.)/(np.power(Zn1,-1.)+1.)
    Tlw1 = (2.*Zn1)/(Zn1+1)
    Tlw2 = (2.*np.power(Zn1,-1.))/(np.power(Zn1,-1.)+1.)
    Zn2 = rho2/rho
    Rrw1 = (Zn2-1.)/(Zn2+1.)
    Rrw2 = (np.power(Zn2,-1.)-1.)/(np.power(Zn2,-1.)+1.)
    Trw1 = (2.*Zn2)/(Zn2+1.)
    Trw2 = (2.*np.power(Zn2,-1.))/(np.power(Zn2,-1.)+1.)

    # reflection and transmission matrix for pressure
    Rmatrix = np.array([[Rlw1, Rlw2],[Rrw1, Rrw2],[Tlw1, Tlw2],[Trw1, Trw2]])
    # reflection and transmission matrix for velocity
    Rmatrixvel = np.array([[-Rlw1, -Rlw2],[-Rrw1, -Rrw2],[Tlw2, Tlw1],[Trw2, Trw1]])

    return Rmatrix, Rmatrixvel

def Rmatrices2D(rholeft,rhoright,rholower,rhoupper,rho):
    # print rholeft,rhoright,rholower,rhoupper,rho
    Znleft = rholeft/rho
    Rlw1 = (Znleft-1.)/(Znleft+1.)
    Rlw2 = (np.power(Znleft,-1.)-1.)/(np.power(Znleft,-1.)+1.)
    Tlw1 = (2.*Znleft)/(Znleft+1)
    Tlw2 = (2.*np.power(Znleft,-1.))/(np.power(Znleft,-1.)+1.)
    Znright = rhoright/rho
    Rrw1 = (Znright-1.)/(Znright+1.)
    Rrw2 = (np.power(Znright,-1.)-1.)/(np.power(Znright,-1.)+1.)
    Trw1 = (2.*Znright)/(Znright+1.)
    Trw2 = (2.*np.power(Znright,-1.))/(np.power(Znright,-1.)+1.)

    Znlower = rholower/rho
    Rlw3 = (Znlower-1.)/(Znlower+1.)
    Rlw4 = (np.power(Znlower,-1.)-1.)/(np.power(Znlower,-1.)+1.)
    Tlw3 = (2.*Znlower)/(Znlower+1)
    Tlw4 = (2.*np.power(Znlower,-1.))/(np.power(Znlower,-1.)+1.)
    Znupper = rhoupper/rho
    Rrw3 = (Znupper-1.)/(Znupper+1.)
    Rrw4 = (np.power(Znupper,-1.)-1.)/(np.power(Znupper,-1.)+1.)
    Trw3 = (2.*Znupper)/(Znupper+1.)
    Trw4 = (2.*np.power(Znupper,-1.))/(np.power(Znupper,-1.)+1.)

    # reflection and transmission matrix for pressure
    Rmatrix = np.array([[Rlw1, Rlw2, Rlw3, Rlw4],[Rrw1, Rrw2, Rrw3, Rrw4],[Tlw1, Tlw2, Tlw3, Tlw4],[Trw1, Trw2, Trw3, Trw4]])
    # reflection and transmission matrix for velocity
    Rmatrixvel = np.array([[-Rlw1, -Rlw2, -Rlw3, -Rlw4],[-Rrw1, -Rrw2, -Rrw3, -Rrw4],[Tlw2, Tlw1, Tlw4, Tlw3],[Trw2, Trw1, Trw4, Trw3]])

    return Rmatrix, Rmatrixvel

def kcalc_old(dx,N,cw,PMLcellsghost):
    # wavenumber discretization
    kmax = np.pi/dx
    dk = kmax/(N/2)               # wave number discretization
    k = np.concatenate((np.arange(0,kmax+dk,dk),  np.arange(kmax-dk,0,-dk)), axis=1)   
    jfact = 1j*np.concatenate((np.ones((1.,N/2.+1.)), -np.ones((1.,N/2.-1.))), axis=1)

    dkgh = kmax/np.power(2.,(ceil(log2(cw/2+PMLcellsghost))))          
    kcy =  np.concatenate((np.arange(0.,kmax+dkgh,dkgh),  np.arange(kmax-dkgh,0,-dkgh)))   #TODO: Why is this missing axis=1 ?
    jfactcy = 1j*np.concatenate((np.ones((1.,np.power(2.,(ceil(log2(cw/2+PMLcellsghost))))+1.)), -np.ones((1.,np.power(2.,(ceil(log2(cw/2+PMLcellsghost))))-1.))), axis=1)

    dkgh2 = kmax/np.power(2.,(ceil(log2(PMLcellsghost))))     
    kgh =  np.concatenate((np.arange(0.,kmax+dkgh2,dkgh2),  np.arange(kmax-dkgh2,0,-dkgh2)), axis=1)  
    jfactgh = 1j*np.concatenate((np.ones((1.,np.power(2.,(ceil(log2(PMLcellsghost))))+1.)), -np.ones((1.,np.power(2.,(ceil(log2(PMLcellsghost))))-1.))), axis=1)
 
    return k, jfact, kcy, jfactcy, kgh, jfactgh

def kcalc(dx,N):
    # wavenumber discretization. We only compute k and jfact anylonger
    kmax = np.pi/dx
    
    A = np.power(2.,(N-1))
    
    dkA = kmax/A
    
    k =    np.concatenate((np.arange(0.,kmax+dkA, dkA),  np.arange(kmax-dkA, 0,-dkA)), axis=1)   
    
    jfact =   1j*np.concatenate((np.ones((1.,A+1.)), -np.ones((1.,A-1.))), axis=1)
    
    return k, jfact

def spatderp3(p2,derfact,Wlength,A,Ns2,N1,N2,Rmatrix,p1,p3,var,direct):

    #print p2.shape, derfact.shape, Wlength, A.shape, Ns2, N1, N2, Rmatrix.shape, p1.shape, p3.shape, var, direct

    # spatial derivative across three media
    
    # --------------|---------------|---------------
    #   subdomain 1 |   subdomain 2 |   subomain 3
    # --------------|---------------|---------------
    #           interface 1         interface 3

    # So far, we assume that only two subdomains meet at 1 interface !!
     
    # Derivatives are computed in subdomain 2
    
    # derfact = factor to compute derivative in wavenumber domain
    # Wlength = length of window function
    # A = window function
    # Ns2 = number of pressure nodes in subdomain 2
    # N1 = # of ffts that are applied
    # N2 = dimension of the ffts
    # Rmatrix = matrix of reflection coefficients
    # p1 = variable matrix subdomain 1
    # p3 = variable matrix subdomain 1
    # var = variable index: 0 for pressure, 1,2,3, for respectively x, z and y (in 3rd dimension) velocity 
    # direct = direction for computation of derivative: 0,1 for z, x direction respectively
    
    
    Ns1 = np.size(p1, axis=direct)
    Ns3 = np.size(p3, axis=direct)
    
    if direct == 0: # transpose variables to compute derivative in right direction
        p1 = p1.transpose()
        p2 = p2.transpose()
        p3 = p3.transpose()
        
  
    if var == 0: # pressure nodes: calculation for variable node staggered with boundary
        # size 123 = number of pressure nodes
        size123 = Wlength*2+Ns2
        Lp = np.zeros((N1,Ns2+1))
        
        G = np.ones((N1,size123))
        G[0:N1,0:np.around(Wlength)] = np.ones((N1,1))*A[0:np.around(Wlength)].transpose()
        G[0:N1,np.around(Wlength)+Ns2:size123] = np.ones((N1,1))*A[np.around(Wlength)+1:np.around(2*Wlength)+1].transpose()
        catemp = np.concatenate((Rmatrix[2,1]*p1[:,Ns1-Wlength:Ns1]+Rmatrix[0,0]*p2[:,Wlength-1::-1], \
                                 p2[:,0:Ns2], \
                                 Rmatrix[3,1]*p3[:,0:Wlength]+Rmatrix[1,0]*p2[:,Ns2-1:Ns2-Wlength-1:-1]), axis=1)*G
        Ktemp = fft(catemp,int(N2), axis=1)
        Ktemp_der = (np.ones((N1,1))*derfact[0:N2]*Ktemp)
        #print "Ktemp = ", Ktemp
        
        Ltemp = ifft(Ktemp_der,int(N2), axis=1)
        Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])

        Debug = False
        if Debug:
            #plot and quit
            np.set_printoptions(threshold=np.nan)
            print "catemp: ",catemp

            import matplotlib.pyplot as plt
            f, a = plt.subplots(4)
            a[0].plot(catemp.transpose())
            a[1].plot(Ktemp.transpose())
            a[2].plot(Ktemp_der.transpose())
            a[3].plot(Ltemp.transpose())
            plt.show()
            raise SystemExit

    elif var > 0: # velocity node: calculation for variable node collocated with boundary
        size123 = Wlength*2+Ns2
        Lp = np.zeros((N1,Ns2-1))
  
        G = np.ones((N1,size123))
        G[0:N1,0:np.around(Wlength)] = np.ones((N1,1))*A[0:np.around(Wlength)].transpose()
        G[0:N1,np.around(Wlength)+Ns2:size123] = np.ones((N1,1))*A[np.around(Wlength)+1:np.around(2*Wlength)+1].transpose()

        Ktemp = fft(np.concatenate((Rmatrix[2,1]*p1[:,Ns1-Wlength-1:Ns1-1]+Rmatrix[0,0]*p2[:,Wlength:0:-1], \
                                 p2[:,0:Ns2], \
                                 Rmatrix[3,1]*p3[:,1:Wlength+1]+Rmatrix[1,0]*p2[:,Ns2-2:Ns2-Wlength-2:-1]), axis=1)*G,int(N2), axis=1)
        Ltemp = ifft((np.ones((N1,1))*derfact[0:N2]*Ktemp),int(N2), axis=1)
        Lp[0:N1,0:Ns2-1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2-1]) 
   
    if direct == 0: # transpose Lp to get variables in right direction
        Lp = Lp.transpose()

    return Lp
@profile
def spatderp3_gpu(p2,derfact,Wlength,A,Ns2,N1,N2,Rmatrix,p1,p3,var,direct,plan,g_bufr=None,g_bufi=None):
    #equivalent of spatderp3(~)
    # derfact = factor to compute derivative in wavenumber domain
    # Wlength = length of window function
    # A = window function
    # Ns2 = number of pressure nodes in subdomain 2
    # N1 = # of ffts that are applied
    # N2 = dimension of the ffts
    # Rmatrix = matrix of reflection coefficients
    # p1 = variable matrix subdomain 1
    # p3 = variable matrix subdomain 1
    # var = variable index: 0 for pressure, 1,2,3, for respectively x, z and y (in 3rd dimension) velocity 
    # direct = direction for computation of derivative: 0,1 for z, x direction respectively
    import pycuda.gpuarray as gpuarray
    import pycuda.driver as cuda

    if N1>50 or N2>128 or g_bufr==None or g_bufi==None:
        g_bufr = cuda.mem_alloc(int(8*N1*nearest_2power(N2)))
        g_bufi = cuda.mem_alloc(int(8*N1*nearest_2power(N2)))

    reusecontext = True

    if reusecontext == False:
        from pyfft.cuda import Plan
        from pycuda.tools import make_default_context
        cuda.init()
        context = make_default_context()
        stream = cuda.Stream()
        plan = Plan(int(N2), dtype=np.float64, context=context, stream=stream, fast_math=False)

    #print p2.shape, derfact.shape, Wlength, A.shape, Ns2, N1, N2, Rmatrix.shape, p1.shape, p3.shape, var, direct

    Ns1 = np.size(p1, axis=direct)
    Ns3 = np.size(p3, axis=direct)

    if direct == 0: # transpose variables to compute derivative in right direction
        p1 = p1.transpose()
        p2 = p2.transpose()
        p3 = p3.transpose()


    if var == 0: # pressure nodes: calculation for variable node staggered with boundary
        # size 123 = number of pressure nodes
        size123 = Wlength*2+Ns2
        Lp = np.zeros((N1,Ns2+1))

        G = np.ones((N1,size123))
        G[0:N1,0:np.around(Wlength)] = np.ones((N1,1))*A[0:np.around(Wlength)].transpose()
        G[0:N1,np.around(Wlength)+Ns2:size123] = np.ones((N1,1))*A[np.around(Wlength)+1:np.around(2*Wlength)+1].transpose()
        
        matemp = (Rmatrix[2,1]*p1[:,Ns1-Wlength:Ns1]+Rmatrix[0,0]*p2[:,Wlength-1::-1], p2[:,0:Ns2],\
                  Rmatrix[3,1]*p3[:,0:Wlength]+Rmatrix[1,0]*p2[:,Ns2-1:Ns2-Wlength-1:-1])
        catemp = np.concatenate(matemp, axis=1)*G   #identical to original until here

        batch_it = True
        np.set_printoptions(threshold=np.nan)
        if batch_it:
            (xshape,yshape) = catemp.shape
            ncatemp = np.concatenate((catemp,np.zeros((N1,N2-yshape))),1)
            nfcatemp = np.ravel(ncatemp)
            ncatemp = ncatemp.transpose() #for plot
            nfcatempim = np.zeros_like(nfcatemp)
            cuda.memcpy_htod(g_bufr, nfcatemp)
            cuda.memcpy_htod(g_bufi, nfcatempim)

            #execute the fft
            plan.execute(g_bufr, g_bufi, batch=N1)
        
            #TEMPORARY solution: get back to cpu to multiply by derivfactor
            Ktempr = np.empty_like(ncatemp)
            Ktempi = np.empty_like(ncatemp)
            cuda.memcpy_dtoh(Ktempr,g_bufr)
            cuda.memcpy_dtoh(Ktempi,g_bufi)

            Ktemp = np.empty(Ktempr.shape, dtype=np.complex128)
            Ktemp.real = Ktempr
            Ktemp.imag = Ktempi

            derpart = (np.ones((N1,1))*derfact[0:N2]*(Ktemp.transpose()))
            nderpartr = np.ravel(derpart.real)
            nderparti = np.ravel(derpart.imag)
            cuda.memcpy_htod(g_bufr, nderpartr)
            cuda.memcpy_htod(g_bufi, nderparti)
        
            #execute the ifft
            plan.execute(g_bufr, g_bufi, inverse=True, batch=N1)
            Ltemp = np.empty_like(ncatemp)
            cuda.memcpy_dtoh(Ltemp, g_bufr)
            Ltemp = Ltemp.transpose() #return to original shape
        
            Lp[0:N1,0:Ns2+1] = np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])
            
        else:
            catemprev = catemp.transpose()  #original np.fft had axis=1, plan works on axis=0
            (xshape, yshape) = catemprev.shape
            ncatemp = np.concatenate((catemprev, np.zeros((N2-xshape,N1))),0)    #plan does not do zero padding for us
            ncatempim = np.zeros_like(ncatemp)
        
            g_bufrl = []
            g_bufil = []
            res_bufrl = []
            res_bufil = []
            for i in xrange(N1):
                g_bufrl.append(cuda.mem_alloc(int(2*8*nearest_2power(N2))))
                g_bufil.append(cuda.mem_alloc(int(2*8*nearest_2power(N2))))
                cuda.memcpy_htod(g_bufrl[i], ncatemp[:,i].copy())
                cuda.memcpy_htod(g_bufil[i], ncatempim[:,i].copy())
                plan.execute(g_bufrl[i], g_bufil[i], inverse=False, batch=1)
                res_bufrl.append(np.empty_like(ncatemp[:,i]))
                res_bufil.append(np.empty_like(ncatempim[:,i]))
                cuda.memcpy_dtoh(res_bufrl[i], g_bufrl[i])
                cuda.memcpy_dtoh(res_bufil[i], g_bufil[i])

            Ktempr = np.asarray(res_bufrl, np.float64).transpose()
            Ktempi = np.asarray(res_bufil, np.float64).transpose()

            Ktemp = np.empty(Ktempr.shape, dtype=np.complex128)
            Ktemp.real = Ktempr
            Ktemp.imag = Ktempi

            derpart = (np.ones((N1,1))*derfact[0:N2]*(Ktemp.transpose())).transpose()
            
            for i in xrange(N1):
                cuda.memcpy_htod(g_bufrl[i], derpart.real[:,i].copy())
                cuda.memcpy_htod(g_bufil[i], derpart.imag[:,i].copy())
                plan.execute(g_bufrl[i], g_bufil[i], inverse=True, batch=1)
                cuda.memcpy_dtoh(res_bufrl[i], g_bufrl[i])
                cuda.memcpy_dtoh(res_bufil[i], g_bufil[i])

            Ltemp = np.asarray(res_bufrl, np.float64) #this also effectively transposes
            Lp[0:N1,0:Ns2+1] = np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])

        Debug = False
        if Debug:
            #plot and quit
            import matplotlib.pyplot as plt
            f, a = plt.subplots(4, sharex=False)
            a[0].plot(ncatemp)
            a[1].plot(Ktemp.real)
            a[2].plot(derpart)
            a[3].plot(Ltemp.transpose())
            plt.show()
            raise SystemExit

    elif var > 0: # velocity node: calculation for variable node collocated with boundary
        size123 = Wlength*2+Ns2
        Lp = np.zeros((N1,Ns2-1))

        G = np.ones((N1,size123))
        G[0:N1,0:np.around(Wlength)] = np.ones((N1,1))*A[0:np.around(Wlength)].transpose()
        G[0:N1,np.around(Wlength)+Ns2:size123] = np.ones((N1,1))*A[np.around(Wlength)+1:np.around(2*Wlength)+1].transpose()

        Ktemp = fft(np.concatenate((Rmatrix[2,1]*p1[:,Ns1-Wlength-1:Ns1-1]+Rmatrix[0,0]*p2[:,Wlength:0:-1], \
                                 p2[:,0:Ns2], \
                                 Rmatrix[3,1]*p3[:,1:Wlength+1]+Rmatrix[1,0]*p2[:,Ns2-2:Ns2-Wlength-2:-1]), axis=1)*G,int(N2), axis=1)
        Ltemp = ifft((np.ones((N1,1))*derfact[0:N2]*Ktemp),int(N2), axis=1)
        Lp[0:N1,0:Ns2-1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2-1])

    if direct == 0: # transpose Lp to get variables in right direction
        Lp = Lp.transpose()

    return Lp

def get_grid_spacing(cnf):
    dxv = np.array([0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.])
    return dxv.compress((dxv<cnf.c1/cnf.freqmax/2.).flat)[-1]

def nearest_2power(n):
    return np.power(2.,(np.ceil(np.log2(n))))

def warn(s):
    print("s")

def output_error(e,output_fn):
    output_fn({'status':'error', 'message':str(e), 'exception':e})

def safe_float(s):
    try: return float(s)
    except: return s

def subsample(a, n):
    final_shape = [int(ceil(float(x)/n)) for x in a.shape]
    padded_shape = [x * int(n) for x in final_shape]
    b = np.zeros(padded_shape)
    b[0:a.shape[0],0:a.shape[1]] = a
    b[:a.shape[0],a.shape[1]:] = a[:,a.shape[1]-1:]
    b[a.shape[0]:,:a.shape[1]] = a[a.shape[0]-1:,:]
    b[a.shape[0]:,a.shape[1]:] = a[-1,-1]
    sh = final_shape[0],n,final_shape[1],n
    return b.reshape(sh).mean(-1).mean(1)

