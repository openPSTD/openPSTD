//
// Created by louis on 16-9-15.
//

#ifndef OPENPSTD_KERNELDOMAIN_H
#define OPENPSTD_KERNELDOMAIN_H

#include <eigen/Eigen/Dense>
#include <map>
#include <string>
#include "kernel_functions.h"
#include "PSTDFile.h"

struct domain_values {
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> u0; //TODO (Louis): change float to T, derive a double and a float class
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> w0;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> p0;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> px0;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> pz0;
};

/**
 * A representation of one domain, as seen by the kernel.
 * TODO (Louis): hide some members in the private section? Not sure if useful
 */
class KernelDomain {
public:
    int id;
    double alpha;
    double impedance;
    double rho;
    int topleft[];
    int size[];
    int bottomright[];
    bool is_pml;
    domain_values current_dvals;
    domain_values prev_dvals;

    KernelDomain *left = nullptr;
    KernelDomain *right = nullptr;
    KernelDomain *top = nullptr;
    KernelDomain *bottom = nullptr;
    KernelDomain *pml_for = nullptr;
    bool is_sec_pml;

    rMatrices2D rho_matrices[]; //TODO generalize to 3d

private:
    PSTDFile *cfg;

public:
    /**
     * Default constructor
     * @param cnf class containing configuration files //TODO
     * @param id identifier for this domain
     * @param alpha alpha of the domain, used in calculating impedance
     * @param topleft coordinates of the top left corner (x,y,(z))
     * @param size lengths of the domain edges (x,y,(z))
     * @param is_pml true if domain is pml domain
     * @param pml_for array of adjacent domains for a PML domain. nullptr if not PML domain.
     */
    KernelDomain(const Config cnf, const int id, const double alpha, const int topleft[],
                 const int size[], const bool is_pml, const KernelDomain *pml_for[] = nullptr);

    /**
     * Calculates the rho matrices for all edges touching another domain
     */
    void calc_rho_matrices();

    /**
     * Checks if a certain point is contained in this domain
     */
    bool contains_point(int point[]);

    /**
     * Method that returns a list of all domains touching this domain
     */
    KernelDomain *neighbour_list();

    ~KernelDomain();
};


#endif //OPENPSTD_KERNELDOMAIN_H
