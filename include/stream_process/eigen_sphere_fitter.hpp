#ifndef __STREAM_PROCESS__EIGEN_SPHERE_FITTER__HPP__
#define __STREAM_PROCESS__EIGEN_SPHERE_FITTER__HPP__

#include "algebraic_sphere.hpp"
#include "neighbor.h"
#include "rt_struct_member.h"

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>

#include <octave/config.h>
#include <octave/EIG.h>
#include <octave/dbleDET.h>

// heavily based on Expe, (c) gael gunnebaud

namespace stream_process
{

enum GenEigenForm { GEF_Ax_lBx, GEF_BAx_lx, GEF_ABx_lx };

template< typename T >
class eigen_sphere_fitter : public algebraic_sphere< T >
{
public:
	eigen_sphere_fitter();
	~eigen_sphere_fitter();
	
	bool fit( const neighbor* neighbors, const double* weights );
	
	vmml::Vector3< T > get_mls_gradient( const neighbor* first_nb, 
		double* derivative_weights, const vmml::Vector3< T >& source );
	
	void set_nb_count( size_t nb_count );
	
protected:
	inline T _compute_determinant( gsl_matrix* matrix );
	T _compute_determinant_gsl( gsl_matrix* matrix );
	T _compute_determinant_octave( gsl_matrix* matrix );
	int _eigen_generalized_sym( gsl_matrix* matA, gsl_matrix* matB,	
		gsl_vector* evals, gsl_matrix* evecs, GenEigenForm form );
	
	size_t _nb_count;
	
	rt_struct_member< vmml::Vector3< float > >	_position;

    static const uint N=5;

    // variable allocation to save memory allocation/deallocation
    // as well to keep some results between the fit and the compmuatation of the gradient
    double mVecU[N];
    double mCovMat[N][N];
    double mLambda;
    
    // used by for the gradient
    mutable double mMatdA[N][N];
    mutable gsl_matrix* mSvdMatV;
    mutable gsl_vector* mSvdVecS;
    mutable gsl_vector* mSvdVecWork;
    
}; // class eigen_sphere_fitter



template< typename T >
eigen_sphere_fitter< T >::eigen_sphere_fitter()
	: _position( "position" )
{
    mSvdMatV = gsl_matrix_alloc(5,5);
    mSvdVecS = gsl_vector_alloc(5);
    mSvdVecWork = gsl_vector_alloc(5);
}



template< typename T >
eigen_sphere_fitter< T >::~eigen_sphere_fitter()
{
    gsl_matrix_free(mSvdMatV);
    gsl_vector_free(mSvdVecS);
    gsl_vector_free(mSvdVecWork);
}



template< typename T >
bool
eigen_sphere_fitter< T >::fit( const neighbor* neighbors, const double* weights )
{
    // the constraint matrix
    static double invC[] = {
        0, 0, 0, 0, -0.5,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        -0.5, 0, 0, 0, 0};
    
	uint nofSamples = _nb_count;//pNeighborhood->getNofFoundNeighbors();
    if ( nofSamples < 5 )
    {
        return false;
    }
    
    gsl_matrix_view invC_view = gsl_matrix_view_array (invC, N, N);
    gsl_matrix_view matrixA_view = gsl_matrix_view_array ((double*)mCovMat, N, N);
    
    // directly fill the covariance matrix A'A
    for(uint i=0 ; i<N ; ++i)
        for(uint j=0 ; j<N ; ++j)
            mCovMat[i][j] = 0.;
    
    double vec[N];

	const neighbor*	current_neighbor	= neighbors;
	const double*		current_weight	= weights;

	vmml::Vector3f pt;
    for (uint k=0; k<nofSamples; k++, ++current_neighbor, ++current_weight )
    {
        //const Vector3& pt = pNeighborhood->getNeighbor(k).position();
        //const vmml::Vector3< T >& pt = current_neighbor->get_point();
        pt = current_neighbor->get_point()->get( _position );
		double w = *current_weight; // FIXME pNeighborhood->getNeighborWeight(k);
        
        vec[0] = 1.0;
        vec[1] = pt.x;
        vec[2] = pt.y;
        vec[3] = pt.z;
        vec[4] = pt.lengthSquared();
        
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mCovMat[i][j] += vec[i]*vec[j]*w;
    }

    // Compute the determinant of the covariance matrix
    //T det = _compute_determinant_gsl( &matrixA_view.matrix );
    //T det = _compute_determinant_octave( &matrixA_view.matrix );
    T det = _compute_determinant( &matrixA_view.matrix );
    
	if (det==0)
    {
        // this means either an exact fit is possible or that the problem is underconstrained
        // then algebraic coeffs == cofactors of A'A
        double cofactors[N];
        double norm = 0.0;
        double sign = ( N % 2 ) ? -1.0 : 1.0;
        gsl_matrix* subMat = gsl_matrix_alloc( N-1 , N-1 );
        for( uint k = 0; k < N; ++k )
        {
            // fill the sub matrix
            uint j1 = 0;
            for(uint j=0 ; j<N ; ++j)
            {
                if(j!=k)
                {
                    for(uint i=0 ; i<N-1 ; ++i)
                    {
                        gsl_matrix_set(subMat, i, j1, gsl_matrix_get(&matrixA_view.matrix, i, j));
                    }
                    j1++;
                }
            }
            cofactors[k] = sign * _compute_determinant( subMat );
			//cofactors[k] = sign * _compute_determinant_octave( subMat );
            //cofactors[k] = sign * _compute_determinant_gsl( subMat );
            norm += cofactors[k]*cofactors[k];
//             det += cofactors[k] * gsl_matrix_get(&matrixA_view.matrix, N-1, k);
            sign = -sign;
        }
        gsl_matrix_free(subMat);
        
        norm = sqrt(norm);
        if (norm<1e-4)
            return false;
        
        // copy the result
        for( size_t k = 0; k < N; ++k )
        {
            algebraic_sphere< T >::_coefficients[k] = cofactors[k];
        }
        
    }
    else
    {
        // use the cholesky decomposition to solve the generalized eigen problem
        // as a simple eigen problem
        gsl_matrix *eigen_vectors = gsl_matrix_alloc(N,N);
        gsl_vector *eigen_values = gsl_vector_alloc(N);
        if(!_eigen_generalized_sym(&invC_view.matrix, &matrixA_view.matrix, eigen_values, eigen_vectors, GEF_ABx_lx))
        {
            std::cerr << "ela::eigen_generalized_sym failed:\n"; 
            //ela::print_matrix(&matrixA_view.matrix);
            std::cerr << "\tnofSamples = " << nofSamples << "\n";
            std::cerr << "\tdet(AA) = " << det << "\n\n";
            return false;
        }

        // search the lowest positive eigen value
        int lowestId = -1;
        for (uint i=0 ; i<N ; ++i)
        {
            double ev = gsl_vector_get(eigen_values, i);
            if(fabs(ev)<1e-9)
                ev = 0;
            if ((ev>0) && (lowestId==-1 || ev<gsl_vector_get(eigen_values, lowestId)))
            {
                lowestId = i; 
            }
        }
        
        // keep the value of the eigen value for the gradient calculation
        mLambda = gsl_vector_get(eigen_values, lowestId);
        
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] = gsl_matrix_get(eigen_vectors, i, lowestId);
        
        double norm = mVecU[1]*mVecU[1]+mVecU[2]*mVecU[2]+mVecU[3]*mVecU[3] - 4.*mVecU[0]*mVecU[N-1];
//         std::cout << "norm = " << norm << "\n";
        norm = 1./sqrtf(norm);
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] *= norm;

        gsl_matrix_free(eigen_vectors);
        gsl_vector_free(eigen_values);
        
        // copy
        for ( uint i=0 ; i < N ; ++i )
            algebraic_sphere< T >::_coefficients[i] = mVecU[i];
    }
    
    algebraic_sphere< T >::determine_as_state();

    return true;

}



template< typename T >
void
eigen_sphere_fitter< T >::set_nb_count( size_t nb_count )
{
	_nb_count = nb_count;
}



template< typename T >
inline 
T
eigen_sphere_fitter< T >::_compute_determinant( gsl_matrix* matrix )
{
    #if 1
	return _compute_determinant_octave( matrix );
    #else
	return _compute_determinant_gsl( matrix );
    #endif
}



template< typename T >
T
eigen_sphere_fitter< T >::_compute_determinant_gsl( gsl_matrix* matrix )
{
    gsl_permutation* p = gsl_permutation_alloc( matrix->size1 );
    int i;
    gsl_matrix* LU = gsl_matrix_alloc( matrix->size1, matrix->size2 );
    gsl_matrix_memcpy( LU, matrix );
    gsl_linalg_LU_decomp ( LU, p, &i );
    double d = gsl_linalg_LU_det( LU,i );
    gsl_matrix_free( LU );
    gsl_permutation_free( p );
    return d;
}



template< typename T >
T
eigen_sphere_fitter< T >::_compute_determinant_octave( gsl_matrix* matrix )
{
    // prepare the matrix for octave
    Matrix octaveMatA( matrix->size1, matrix->size2 );
    for ( uint i = 0; i< matrix->size1; ++i )
        for ( uint j = 0; j < matrix->size2 ; ++j )
            octaveMatA.elem( i, j ) = gsl_matrix_get( matrix, i, j );
        
    int info;
    double rcond = 0.0;
    DET det = octaveMatA.determinant(info, rcond);
    volatile double xrcond = rcond;
    xrcond += 1.0;
    return ((info == -1 || xrcond == 1.0) ? 0.0 : det.value ());
}



template< typename T >
int
eigen_sphere_fitter< T >::_eigen_generalized_sym(gsl_matrix* matA, gsl_matrix* matB, gsl_vector* evals, gsl_matrix* evecs, GenEigenForm form)
{
    size_t pbSize = matA->size1;
    assert(matA->size2==pbSize);
    assert(matB->size1==pbSize);
    assert(matB->size2==pbSize);
    assert(evecs->size1==pbSize);
    assert(evecs->size2==pbSize);
    assert(evals->size==pbSize);
    
    // Cholesky of matB -> U'U
    gsl_matrix *matU = gsl_matrix_alloc(pbSize,pbSize);
    gsl_matrix_memcpy(matU, matB);
    if(gsl_linalg_cholesky_decomp(matU))
        return 0;
    
    // clean the strict lower triangular part
    for (uint m=1 ; m<pbSize ; ++m)
        for (uint n=0 ; n<m ; ++n)
            gsl_matrix_set(matU, m, n, 0);
    
    gsl_matrix *matC = gsl_matrix_alloc(pbSize,pbSize);
    gsl_matrix *matAux = gsl_matrix_alloc(pbSize,pbSize);
    gsl_eigen_symmv_workspace *workspace = gsl_eigen_symmv_alloc(pbSize);
    
    if(form==GEF_ABx_lx)
    {
        // compute C = UAU'
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matU, matA, 0.0, matAux);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, matAux, matU, 0.0, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = inv(U) * evecs
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, evecs);
    }
    else if(form==GEF_BAx_lx)
    {
        // compute C = UAU'
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matU, matA, 0.0, matAux);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, matAux, matU, 0.0, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = U' * evecs
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, matU, evecs, 0.0, evecs);
    }
    else if(form==GEF_Ax_lBx)
    {
        // compute C = inv(U').A.inv(U)
        gsl_matrix_memcpy(matC, matA);
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasTrans, CblasNonUnit, 1.0, matU, matC);
        gsl_blas_dtrsm(CblasRight, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = inv(U) * evecs
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, evecs);
    }
    else
    {
        //LOG_ERROR("eigen_generalized_sym: invalid form");
        std::cout << "eigen_generalized_sym: invalid form" << std::endl;
		gsl_matrix_free(matU);
        gsl_matrix_free(matC);
        gsl_eigen_symmv_free(workspace);
        gsl_matrix_free(matAux);
        return 0;
    }
    
    gsl_matrix_free(matU);
    gsl_matrix_free(matC);
    gsl_matrix_free(matAux);
    gsl_eigen_symmv_free(workspace);
    
    return 1;
}



template< typename T >
vmml::Vector3< T >
eigen_sphere_fitter< T >::get_mls_gradient( const neighbor* neighbors, 
	double* derivative_weights, const vmml::Vector3< T >& source )
{
    double matC[] = {
        0, 0, 0, 0,-2,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
       -2, 0, 0, 0, 0};

    // offset used to handle a half matrix
    uint nofSamples = _nb_count;//pNeighborhood->getNofFoundNeighbors();

    vmml::Vector3< T > grad;
    // for each coordinate
    for (uint k=0 ; k<3 ; ++k)
    {
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mMatdA[i][j] = 0.;
        
        double vec[N];
		
		neighbor* current_nb = neighbors;
		double*	current_dw	= derivative_weights;
        for (uint i=0; i<nofSamples; i++, ++current_nb, ++current_dw )
        {
            //Vector3 p = pNeighborhood->getNeighbor(i).position();
            vmml::Vector3< T > position = current_nb->get_point();
            double dwk = 2.0 * ( source[k] - position[k] ) * (*current_dw);
			//pNeighborhood->getNeighborDerivativeWeight(i);
            
            vec[0] = 1.;
            vec[1] = position.x;
            vec[2] = position.y;
            vec[3] = position.z;
            vec[4] = position.lengthSquared();
            
            for(uint i1=0 ; i1<N ; ++i1)
                for(uint j1=0 ; j1<N ; ++j1)
                    mMatdA[i1][j1] += vec[i1] * vec[j1] * dwk;
        }

        // compute dlambda = x^T dA x
        double aux[N];
        double dlambda = 0.;
        for (uint i=0 ; i<N ; ++i)
        {
            aux[i] = 0.;
            for (uint j=0 ; j<N ; ++j)
                aux[i] += mMatdA[i][j] * mVecU[j];
        }
        for (uint i=0 ; i<N ; ++i)
            dlambda += aux[i]*mVecU[i];
        
        // mMatdA <- dA - dlambda * C
        // mMatA <- A - lambda * C
//         for (uint i=1 ; i<N-1 ; ++i)
//         {
//             mMatdA[i][i] -= dlambda;
//             mMatA[i][i]  -= mLambda;
//         }
//         mMatdA[0][N-1] -= 2.*dlambda;
//         mMatA[0][N-1] -= 2.*mLambda;
//         mMatdA[N-1][0] -= 2.*dlambda;
//         mMatA[N-1][0] -= 2.*mLambda;

        double dlcda[N][N]; // = dlambda * C - dA
        double alc[N][N]; // = A - lambda * C
        for (uint i=0 ; i<N ; ++i)
        {
            for (uint j=0 ; j<N ; ++j)
            {
                dlcda[i][j] = dlambda*matC[i*N+j] - mMatdA[i][j];
                alc[i][j] = mCovMat[i][j] - mLambda*matC[i*N+j];
            }
        }
        
        // vecB = dlcda * x (=aux)
        for (uint i=0 ; i<N ; ++i)
        {
            aux[i] = 0.;
            for (uint j=0 ; j<N ; ++j)
                aux[i] += dlcda[i][j] * mVecU[j];
        }
        
        // dX/de_k =  inv(alc) * dlcda * x = inv(alc) * vecB
        // but since alc is singular we use a SVD as follow:
        
        double dU[N]; // du/dx_k =  inv(alc) * dlcda * u
        gsl_vector_view dU_view = gsl_vector_view_array((double*)dU, N);
        gsl_vector_view vecB_view = gsl_vector_view_array((double*)aux, N);
        gsl_matrix_view alc_view = gsl_matrix_view_array((double*)alc, N, N);
        
        gsl_linalg_SV_decomp(&alc_view.matrix, mSvdMatV, mSvdVecS, mSvdVecWork);
        gsl_linalg_SV_solve (&alc_view.matrix, mSvdMatV, mSvdVecS, &vecB_view.vector, &dU_view.vector);
        
//         for (uint i=1 ; i<N-1 ; ++i)
//             dU[i] = -dU[i];


//         EigenvectorDerivativeFramework<5> solver;
//         double dU[N];
//         solver.solve(mMatA, mMatdA, mLambda, mVecX, dU);

        grad[k] = dU[0] + dU[1]*source[0]+dU[2]*source[1]+dU[3]*source[2] + dU[4]*source.squaredLength()
            + mVecU[1+k] + 2.*source[k]*mVecU[4];
    }

    return grad;
}

} // namespace stream_process

#endif

