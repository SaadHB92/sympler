/*
 * This file is part of the SYMPLER package.
 * https://github.com/kauzlari/sympler
 *
 * Copyright 2002-2013, 
 * David Kauzlaric <david.kauzlaric@frias.uni-freiburg.de>,
 * and others authors stated in the AUTHORS file in the top-level 
 * source directory.
 *
 * SYMPLER is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SYMPLER is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SYMPLER.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Please cite the research papers on SYMPLER in your own publications. 
 * Check out the PUBLICATIONS file in the top-level source directory.
 *
 * You are very welcome to contribute extensions to the code. Please do 
 * so by making a pull request on https://github.com/kauzlari/sympler
 * 
 */



#include "gm_vector.h"

#include "phase.h"
#include "simulation.h"
#include "data_format.h"
#include "grid_averager.h"

GridMeter_Register<GridMeterVector> grid_meter_vector("Vector");


/*--- GridMeterScalar ---*/

GridMeterVector::GridMeterVector(GridAverager *averager): GridMeter(averager)
{
  init();
}


void GridMeterVector::init()
{
  m_properties.setClassName("Vector");

  m_properties.setDescription("Measure a vector field.");

  STRINGPC
      (symbol, m_name,
       "Name of the vector field to measure.");

  INTPC
      (moment, m_moment, 0,
       "Which moment to measure of the vector field. The usual maximum "
           "likelyhood estimation m_i = sum x^i/N is used.");

  m_name = "vector";
  m_moment = 1;
}


void GridMeterVector::setup()
{
  GridMeter::setup();

  if (m_colour == ALL_COLOURS)
    throw gError("GridMeterVector::setup", "Please specify a species.");

  m_moment_o = 
      M_GRID_AVERAGER->format().addAttribute
      (m_species + STR_DELIMITER + m_name + "^" + ObjToString(m_moment),
       DataFormat::VECTOR_POINT).offset;

  m_offset = Particle::s_tag_format[m_colour].attrByName(m_name).offset;
  MSG_DEBUG("GridMeterVector::setup", "m_offset = " << m_offset << ", m_name = " << m_name);
}


void GridMeterVector::measure(data_sp data)
{
  Phase *phase = ((Simulation*) M_GRID_AVERAGER->parent())->phase();
  
  vector<point_t> *moments = data->vectorPointByOffset(m_moment_o).value();
  int n_cells = M_GRID_AVERAGER->nCells();

  moments->resize(n_cells);

  FOR_EACH_FREE_PARTICLE_C
      (phase, m_colour,
       int index = M_GRID_AVERAGER->location(c, __iSLFE->mySlot);
       int n = M_GRID_AVERAGER->nParticles(c, index);
         
       if (n) {
         point_t e = __iSLFE->tag.pointByOffset(m_offset);
         for(size_t i = 0; i < SPACE_DIMS; ++i)
         /* fixme!!! Slow! Better divide only once per cell. */
          (*moments)[index][i] += pow(e[i], m_moment)/n;
       }
      );
}

