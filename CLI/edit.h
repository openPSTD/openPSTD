//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//      20-1-2016
//
// Authors:
//      Michiel Fortuijn
//      Louis van Harten
//      Omar  Richardson
//
// Purpose:
//      Editing command from the command line.
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_EDIT_CLI_H
#define OPENPSTD_EDIT_CLI_H

#include <boost/program_options.hpp>
#include <kernel/KernelInterface.h>

namespace OpenPSTD
{
    namespace CLI
    {
        namespace po = boost::program_options;

        struct CLIDomainAdd
        {
        public:
            CLIDomainAdd()
                    : x1(0), y1(0), x2(0), y2(0)
            { }
            CLIDomainAdd(float x1, float y1, float x2, float y2)
                    : x1(x1), y1(y1), x2(x2), y2(y2)
            { };
            float x1, y1, x2, y2;
        };

        struct CLIDomainChange
        {
        public:
            CLIDomainChange()
                    : id(-1), x1(0), y1(0), x2(0), y2(0)
            { }

            CLIDomainChange(int id, float x1, float y1, float x2, float y2)
                    : id(id), x1(x1), y1(y1), x2(x2), y2(y2)
            { };
            int id;
            float x1, y1, x2, y2;
        };

        struct CLIEdgeAbsorption
        {
        public:
            int id;
            char edge;
            float value;

            CLIEdgeAbsorption()
                    : id(-1), edge('t'), value(0)
            {  }

            CLIEdgeAbsorption(int id, char edge, float value)
                    : id(id), edge(edge), value(value)
            { }
        };

        struct CLIEdgeLR
        {
        public:
            int id;
            char edge;
            bool value;

            CLIEdgeLR()
                    : id(-1), edge('t'), value(true)
            {  }

            CLIEdgeLR(int id, char edge, float value)
                    : id(id), edge(edge), value(value)
            { }
        };

        struct CLISpeakerReceiverAdd
        {
        public:
            float x, y;

            CLISpeakerReceiverAdd()
                    : x(0), y(0)
            { }

            CLISpeakerReceiverAdd(float x, float y)
                    : x(x), y(y)
            { }
        };

        class EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option) = 0;
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input) = 0;
        };

        class AddDomainEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class RemoveDomainEditCommandPart: public EditCommandPart
        {
        private:
            int delIndex;

        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class ChangeDomainEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class ChangeEdgeAbsorptionEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class ChangeEdgeLREditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class AddSpeakerEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class RemoveSpeakerEditCommandPart: public EditCommandPart
        {
        private:
            int delIndex;

        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class AddReceiverEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class RemoveReceiverEditCommandPart: public EditCommandPart
        {
        private:
            int delIndex;

        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

        class SetOptionEditCommandPart: public EditCommandPart
        {
        public:
            virtual void AddOptions(po::options_description_easy_init add_option);
            virtual void Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, po::variables_map input);
        };

#endif //OPENPSTD_EDIT_CLI_H

    }
}
