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
        
        Ltemp = ifft(Ktemp_der,int(N2), axis=1)
        Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])

 
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

def spatderp3_cuda(p2,derfact,Wlength,A,Ns2,N1,N2,Rmatrix,p1,p3,var,direct,context,stream,plan_set,g_bufl,mulfunc,use_32bit):
    # equivalent of spatderp3(~)
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
    import pycuda.driver as cuda
    
    if use_32bit:
        if 4*N1*int(N2) > g_bufl["m_size"]:
            g_bufl["mr"] = cuda.mem_alloc(int(4*N1*int(N2)))
            g_bufl["mi"] = cuda.mem_alloc(int(4*N1*int(N2)))
            g_bufl["m1"] = cuda.mem_alloc(int(4*N1*int(N2)))
            g_bufl["m2"] = cuda.mem_alloc(int(4*N1*int(N2)))
            g_bufl["m3"] = cuda.mem_alloc(int(4*N1*int(N2)))
            g_bufl["m_size"] = int(4*N1*int(N2))

        if 4*int(N2) > g_bufl["d_size"] or A.size > g_bufl["d_size"]:
            g_bufl["dr"] = cuda.mem_alloc(np.maximum(4*int(N2),A.size))
            g_bufl["di"] = cuda.mem_alloc(np.maximum(4*int(N2),A.size))
            g_bufl["d_size"] = np.maximum(4*int(N2),A.size)

        if str(int(N2)) not in plan_set.keys():
            from pyfft.cuda import Plan
            plan_set[str(int(N2))] = Plan(int(N2), dtype=np.float32, context=context, stream=stream, fast_math=False)

        Ns1 = np.size(p1, axis=direct)
        Ns3 = np.size(p3, axis=direct)

        p1 = p1.astype(np.float32)
        p2 = p2.astype(np.float32)
        p3 = p3.astype(np.float32)
        A = A.astype(np.float32)
        derfact = derfact.astype(np.complex64)
        Rmatrix = Rmatrix.astype(np.float32)

        if direct == 0: # transpose variables to compute derivative in right direction
            p1 = p1.transpose()
            p2 = p2.transpose()
            p3 = p3.transpose()

        if var == 0: # pressure nodes: calculation for variable node staggered with boundary

            #prepare data needed for applying window function
            cuda.memcpy_htod(g_bufl["m1"], np.ravel(p1))
            cuda.memcpy_htod(g_bufl["m2"], np.ravel(p2))
            cuda.memcpy_htod(g_bufl["m3"], np.ravel(p3))
            cuda.memcpy_htod(g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(np.maximum(nearest_2power(N1)/blksize, 1))

            mulfunc["pres_window"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float32(Rmatrix[2,1]), \
                np.float32(Rmatrix[0,0]), np.float32(Rmatrix[3,1]), np.float32(Rmatrix[1,0]), block=(blksize,blksize,1), grid=(grdx,grdy))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cuda.memcpy_htod(g_bufl["dr"], np.ravel(derfact.real))
            cuda.memcpy_htod(g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1), block=(blksize,blksize,1), grid=(grdx,grdy))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float32)
            cuda.memcpy_dtoh(Ltemp, g_bufl["mr"])

            #Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])
            Lp = Ltemp[:,Wlength:Wlength+Ns2+1]

        elif var > 0: # velocity node: calculation for variable node collocated with boundary

            #prepare data needed for applying window function
            cuda.memcpy_htod(g_bufl["m1"], np.ravel(p1))
            cuda.memcpy_htod(g_bufl["m2"], np.ravel(p2))
            cuda.memcpy_htod(g_bufl["m3"], np.ravel(p3))
            cuda.memcpy_htod(g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(nearest_2power(N1)/blksize)

            mulfunc["velo_window"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float32(Rmatrix[2,1]), \
                np.float32(Rmatrix[0,0]), np.float32(Rmatrix[3,1]), np.float32(Rmatrix[1,0]), block=(blksize,blksize,1), grid=(grdx,grdy))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cuda.memcpy_htod(g_bufl["dr"], np.ravel(derfact.real))
            cuda.memcpy_htod(g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1), block=(blksize,blksize,1), grid=(grdx,grdy))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float32)
            cuda.memcpy_dtoh(Ltemp, g_bufl["mr"])

            Lp = Ltemp[:,Wlength:Wlength+Ns2-1]

        if direct == 0: # transpose Lp to get variables in right direction
            Lp = Lp.transpose()

        return Lp

    else:
        if 8*N1*int(N2) > g_bufl["m_size"]:
            g_bufl["mr"] = cuda.mem_alloc(int(8*N1*int(N2)))
            g_bufl["mi"] = cuda.mem_alloc(int(8*N1*int(N2)))
            g_bufl["m1"] = cuda.mem_alloc(int(8*N1*int(N2)))
            g_bufl["m2"] = cuda.mem_alloc(int(8*N1*int(N2)))
            g_bufl["m3"] = cuda.mem_alloc(int(8*N1*int(N2)))
            g_bufl["m_size"] = int(8*N1*int(N2))

        if 8*int(N2) > g_bufl["d_size"] or A.size > g_bufl["d_size"]:
            g_bufl["dr"] = cuda.mem_alloc(np.maximum(8*int(N2),A.size))
            g_bufl["di"] = cuda.mem_alloc(np.maximum(8*int(N2),A.size))
            g_bufl["d_size"] = np.maximum(8*int(N2),A.size)

        if str(int(N2)) not in plan_set.keys():
            from pyfft.cuda import Plan
            plan_set[str(int(N2))] = Plan(int(N2), dtype=np.float64, context=context, stream=stream, fast_math=False)

        Ns1 = np.size(p1, axis=direct)
        Ns3 = np.size(p3, axis=direct)

        if direct == 0: # transpose variables to compute derivative in right direction
            p1 = p1.transpose()
            p2 = p2.transpose()
            p3 = p3.transpose()

        if var == 0: # pressure nodes: calculation for variable node staggered with boundary

            #prepare data needed for applying window function
            cuda.memcpy_htod(g_bufl["m1"], np.ravel(p1))
            cuda.memcpy_htod(g_bufl["m2"], np.ravel(p2))
            cuda.memcpy_htod(g_bufl["m3"], np.ravel(p3))
            cuda.memcpy_htod(g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(np.maximum(nearest_2power(N1)/blksize, 1))

            mulfunc["pres_window"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float64(Rmatrix[2,1]), \
                np.float64(Rmatrix[0,0]), np.float64(Rmatrix[3,1]), np.float64(Rmatrix[1,0]), block=(blksize,blksize,1), grid=(grdx,grdy))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cuda.memcpy_htod(g_bufl["dr"], np.ravel(derfact.real))
            cuda.memcpy_htod(g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1), block=(blksize,blksize,1), grid=(grdx,grdy))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float64)
            cuda.memcpy_dtoh(Ltemp, g_bufl["mr"])

            #Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])
            Lp = Ltemp[:,Wlength:Wlength+Ns2+1]

        elif var > 0: # velocity node: calculation for variable node collocated with boundary

            #prepare data needed for applying window function
            cuda.memcpy_htod(g_bufl["m1"], np.ravel(p1))
            cuda.memcpy_htod(g_bufl["m2"], np.ravel(p2))
            cuda.memcpy_htod(g_bufl["m3"], np.ravel(p3))
            cuda.memcpy_htod(g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(nearest_2power(N1)/blksize)

            mulfunc["velo_window"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float64(Rmatrix[2,1]), \
                np.float64(Rmatrix[0,0]), np.float64(Rmatrix[3,1]), np.float64(Rmatrix[1,0]), block=(blksize,blksize,1), grid=(grdx,grdy))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cuda.memcpy_htod(g_bufl["dr"], np.ravel(derfact.real))
            cuda.memcpy_htod(g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1), block=(blksize,blksize,1), grid=(grdx,grdy))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float64)
            cuda.memcpy_dtoh(Ltemp, g_bufl["mr"])

            Lp = Ltemp[:,Wlength:Wlength+Ns2-1]

        if direct == 0: # transpose Lp to get variables in right direction
            Lp = Lp.transpose()

        return Lp

#@profile
def spatderp3_ocl(p2,derfact,Wlength,A,Ns2,N1,N2,Rmatrix,p1,p3,var,direct,context,queue,plan_set,g_bufl,mulfunc,use_32bit):
    # equivalent of spatderp3(~)
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
    import pyopencl as cl

    if use_32bit:
        if 4*N1*int(N2) > g_bufl["m_size"]:
            mf = cl.mem_flags
            g_bufl["mr"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=4*N1*int(N2))
            g_bufl["mi"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=4*N1*int(N2))
            g_bufl["m1"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=4*N1*int(N2))
            g_bufl["m2"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=4*N1*int(N2))
            g_bufl["m3"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=4*N1*int(N2))
            g_bufl["m_size"] = int(4*N1*int(N2))

        if 4*int(N2) > g_bufl["d_size"] or A.size > g_bufl["d_size"]:
            mf = cl.mem_flags
            g_bufl["dr"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=np.maximum(4*int(N2),A.size))
            g_bufl["di"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=np.maximum(4*int(N2),A.size))
            g_bufl["d_size"] = np.maximum(4*int(N2),A.size)

        if str(int(N2)) not in plan_set.keys():
            from pyfft.cl import Plan
            plan_set[str(int(N2))] = Plan(int(N2), dtype=np.float32, context=context, fast_math=False)

        Ns1 = np.size(p1, axis=direct)
        Ns3 = np.size(p3, axis=direct)

        p1 = p1.astype(np.float32)
        p2 = p2.astype(np.float32)
        p3 = p3.astype(np.float32)
        A = A.astype(np.float32)
        derfact = derfact.astype(np.complex64)
        Rmatrix = Rmatrix.astype(np.float32)

        if direct == 0: # transpose variables to compute derivative in right direction
            p1 = p1.transpose()
            p2 = p2.transpose()
            p3 = p3.transpose()

        if var == 0: # pressure nodes: calculation for variable node staggered with boundary

            #prepare data needed for applying window function
            cl.enqueue_copy(queue, g_bufl["m1"], np.ravel(p1))
            cl.enqueue_copy(queue, g_bufl["m2"], np.ravel(p2))
            cl.enqueue_copy(queue, g_bufl["m3"], np.ravel(p3))
            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(np.maximum(nearest_2power(N1)/blksize, 1))

            mulfunc["pres_window"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float32(Rmatrix[2,1]), \
                np.float32(Rmatrix[0,0]), np.float32(Rmatrix[3,1]), np.float32(Rmatrix[1,0]))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(derfact.real))
            cl.enqueue_copy(queue, g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float32)
            cl.enqueue_copy(queue, Ltemp, g_bufl["mr"])

            #Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])
            Lp = Ltemp[:,Wlength:Wlength+Ns2+1]

        elif var > 0: # velocity node: calculation for variable node collocated with boundary

            #prepare data needed for applying window function
            cl.enqueue_copy(queue, g_bufl["m1"], np.ravel(p1))
            cl.enqueue_copy(queue, g_bufl["m2"], np.ravel(p2))
            cl.enqueue_copy(queue, g_bufl["m3"], np.ravel(p3))
            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(nearest_2power(N1)/blksize)

            mulfunc["velo_window"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float32(Rmatrix[2,1]), \
                np.float32(Rmatrix[0,0]), np.float32(Rmatrix[3,1]), np.float32(Rmatrix[1,0]))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(derfact.real))
            cl.enqueue_copy(queue, g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float32)
            cl.enqueue_copy(queue, Ltemp, g_bufl["mr"])

            Lp = Ltemp[:,Wlength:Wlength+Ns2-1]

        if direct == 0: # transpose Lp to get variables in right direction
            Lp = Lp.transpose()

        return Lp

    else:
        if 8*N1*int(N2) > g_bufl["m_size"]:
            mf = cl.mem_flags
            g_bufl["mr"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=8*N1*int(N2))
            g_bufl["mi"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=8*N1*int(N2))
            g_bufl["m1"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=8*N1*int(N2))
            g_bufl["m2"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=8*N1*int(N2))
            g_bufl["m3"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=8*N1*int(N2))
            g_bufl["m_size"] = int(8*N1*int(N2))

        if 8*int(N2) > g_bufl["d_size"] or A.size > g_bufl["d_size"]:
            mf = cl.mem_flags
            g_bufl["dr"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=np.maximum(8*int(N2),A.size))
            g_bufl["di"] = cl.Buffer(context, mf.ALLOC_HOST_PTR, size=np.maximum(8*int(N2),A.size))
            g_bufl["d_size"] = np.maximum(8*int(N2),A.size)

        if str(int(N2)) not in plan_set.keys():
            from pyfft.cl import Plan
            plan_set[str(int(N2))] = Plan(int(N2), dtype=np.float64, context=context, fast_math=False)

        Ns1 = np.size(p1, axis=direct)
        Ns3 = np.size(p3, axis=direct)

        if direct == 0: # transpose variables to compute derivative in right direction
            p1 = p1.transpose()
            p2 = p2.transpose()
            p3 = p3.transpose()

        if var == 0: # pressure nodes: calculation for variable node staggered with boundary

            #prepare data needed for applying window function
            cl.enqueue_copy(queue, g_bufl["m1"], np.ravel(p1))
            cl.enqueue_copy(queue, g_bufl["m2"], np.ravel(p2))
            cl.enqueue_copy(queue, g_bufl["m3"], np.ravel(p3))
            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(np.maximum(nearest_2power(N1)/blksize, 1))

            mulfunc["pres_window"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float64(Rmatrix[2,1]), \
                np.float64(Rmatrix[0,0]), np.float64(Rmatrix[3,1]), np.float64(Rmatrix[1,0]))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(derfact.real))
            cl.enqueue_copy(queue, g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float64)
            cl.enqueue_copy(queue, Ltemp, g_bufl["mr"])

            #Lp[0:N1,0:Ns2+1] =  np.real(Ltemp[0:N1,Wlength:Wlength+Ns2+1])
            Lp = Ltemp[:,Wlength:Wlength+Ns2+1]

        elif var > 0: # velocity node: calculation for variable node collocated with boundary

            #prepare data needed for applying window function
            cl.enqueue_copy(queue, g_bufl["m1"], np.ravel(p1))
            cl.enqueue_copy(queue, g_bufl["m2"], np.ravel(p2))
            cl.enqueue_copy(queue, g_bufl["m3"], np.ravel(p3))
            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(A))

            blksize = 8
            grdx = int(N2)/blksize
            grdy = int(nearest_2power(N1)/blksize)

            mulfunc["velo_window"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["m1"], g_bufl["m2"], g_bufl["m3"], \
                np.int32(np.around(Wlength)), np.int32(Ns1), np.int32(Ns2), np.int32(Ns3), np.int32(N2), np.int32(N1), np.float64(Rmatrix[2,1]), \
                np.float64(Rmatrix[0,0]), np.float64(Rmatrix[3,1]), np.float64(Rmatrix[1,0]))

            #select the N2 length plan from the set and execute the fft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], batch=N1)

            cl.enqueue_copy(queue, g_bufl["dr"], np.ravel(derfact.real))
            cl.enqueue_copy(queue, g_bufl["di"], np.ravel(derfact.imag))

            mulfunc["derifact"](queue, (grdx,grdy), None, g_bufl["mr"], g_bufl["mi"], g_bufl["dr"], g_bufl["di"], np.int32(N2), np.int32(N1))

            #execute the ifft
            plan_set[str(int(N2))].execute(g_bufl["mr"], g_bufl["mi"], inverse=True, batch=N1)

            Ltemp = np.empty((N1,N2), dtype=np.float64)
            cl.enqueue_copy(queue, Ltemp, g_bufl["mr"])

            Lp = Ltemp[:,Wlength:Wlength+Ns2-1]

        if direct == 0: # transpose Lp to get variables in right direction
            Lp = Lp.transpose()

        return Lp

def get_grid_spacing(cnf):
    dxv = np.array([0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.])
    return dxv.compress((dxv<cnf.c1/cnf.freqmax/2.).flat)[-1]

def nearest_2power(n):
    return np.power(2.,(np.ceil(np.log2(n))))
        
