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
// Date: 20-1-2016
//
//
// Authors: M R Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#include "edit.h"
#include <boost/regex.hpp>

namespace OpenPSTD
{
    namespace CLI
    {
        using namespace boost;
        using namespace boost::program_options;

        void validate(boost::any &v, const std::vector<std::string> &values, CLIDomainAdd *, int)
        {
            static regex r("\\[([^,]*),([^,]*),([^,]*),([^,]*)\\]");

            validators::check_first_occurrence(v);
            const std::string &s = validators::get_single_string(values);

            smatch match;
            if (regex_match(s, match, r))
            {
                v = any(CLIDomainAdd(lexical_cast<int>(match[1]), lexical_cast<float>(match[2]),
                                     lexical_cast<float>(match[3]),
                                     lexical_cast<float>(match[4])));
            }
            else
            {
                throw validation_error(validation_error::invalid_option_value);
            }
        }

        void validate(boost::any &v, const std::vector<std::string> &values, CLIDomainChange *, int)
        {
            static regex r("\\(([^,]*),\\[([^,]*),([^,]*),([^,]*),([^,]*)\\]\\)");

            validators::check_first_occurrence(v);
            const std::string &s = validators::get_single_string(values);

            smatch match;
            if (regex_match(s, match, r))
            {
                v = any(CLIDomainChange(lexical_cast<int>(match[1]), lexical_cast<float>(match[2]),
                                        lexical_cast<float>(match[3]),
                                        lexical_cast<float>(match[4]), lexical_cast<float>(match[5])));
            }
            else
            {
                throw validation_error(validation_error::invalid_option_value);
            }
        }

        void validate(boost::any &v, const std::vector<std::string> &values, CLIEdgeAbsorption *, int)
        {
            static regex r("\\(([^,]*),([tblr]),([^,]*)\\)");

            validators::check_first_occurrence(v);
            const std::string &s = validators::get_single_string(values);

            smatch match;
            if (regex_match(s, match, r))
            {
                char edge;
                if (!match[2].compare("l")) edge = 'l';
                else if (!match[2].compare("r")) edge = 'r';
                else if (!match[2].compare("t")) edge = 't';
                else if (!match[2].compare("b")) edge = 'b';
                v = any(CLIEdgeAbsorption(lexical_cast<int>(match[1]), edge, lexical_cast<float>(match[3])));
            }
            else
            {
                throw validation_error(validation_error::invalid_option_value);
            }
        }

        void validate(boost::any &v, const std::vector<std::string> &values, CLIEdgeLR *, int)
        {
            static regex r("\\(([^,]*),([tdlr]),(true)|(false)\\)");

            validators::check_first_occurrence(v);
            const std::string &s = validators::get_single_string(values);

            smatch match;
            if (regex_match(s, match, r))
            {
                char edge;
                if (match[2].compare("l")) edge = 'l';
                else if (match[2].compare("r")) edge = 'r';
                else if (match[2].compare("t")) edge = 't';
                else if (match[2].compare("b")) edge = 'b';
                v = any(CLIEdgeLR(lexical_cast<int>(match[1]), edge, match[3].matched));
            }
            else
            {
                throw validation_error(validation_error::invalid_option_value);
            }
        }

        void validate(boost::any &v, const std::vector<std::string> &values, CLISpeakerReceiverAdd *, int)
        {
            static regex r("\\[([^,]*),([^,]*)\\]");

            validators::check_first_occurrence(v);
            const std::string &s = validators::get_single_string(values);

            smatch match;
            if (regex_match(s, match, r))
            {
                v = any(CLISpeakerReceiverAdd(lexical_cast<int>(match[1]), lexical_cast<float>(match[2])));
            }
            else
            {
                throw validation_error(validation_error::invalid_option_value);
            }
        }

        void AddDomainEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("domain-add,d",
                       value<std::vector<CLIDomainAdd>>(),
                       "Add a domain, --domain-add <x,y,size-x,size-y>, eg --domain-add [0,0,10,10]");
        }

        void AddDomainEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("domain-add") == 0)
                return;

            std::vector<CLIDomainAdd> add = input["domain-add"].as<std::vector<CLIDomainAdd>>();

            for (int i = 0; i < add.size(); ++i)
            {
                Kernel::DomainConf d;
                d.TopLeft = QVector2D(add[i].x1, add[i].y1);
                d.Size = QVector2D(add[i].x2, add[i].y2);
                d.T.Absorption = 1;
                d.B.Absorption = 1;
                d.L.Absorption = 1;
                d.R.Absorption = 1;

                d.T.LR = false;
                d.B.LR = false;
                d.L.LR = false;
                d.R.LR = false;

                model->Domains.push_back(d);
            }
        }

        void RemoveDomainEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("domain-remove,D",
                       value<int>(&delIndex)->default_value(-1),
                       "Remove a domain, domain id are always subsequent, --domain-remove id");
        }

        void RemoveDomainEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (delIndex == -1)
                return;

            model->Domains.erase(model->Domains.begin() + delIndex);
        }

        void ChangeDomainEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("domain-change,c",
                       value<std::vector<CLIDomainChange>>(),
                       "Change the positions of a domain, --domain-add (id,[x,y,size-x,size-y]), eg --domain-add (1,[0,0,10,10])");
        }

        void ChangeDomainEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("domain-change") == 0)
                return;

            std::vector<CLIDomainChange> add = input["domain-change"].as<std::vector<CLIDomainChange>>();

            for (int i = 0; i < add.size(); ++i)
            {
                model->Domains[add[i].id].TopLeft = QVector2D(add[i].x1, add[i].y1);
                model->Domains[add[i].id].Size = QVector2D(add[i].x2, add[i].y2);
            }
        }

        void ChangeEdgeAbsorptionEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("edge-absorption,a",
                       value<std::vector<CLIEdgeAbsorption>>(),
                       "change the absorption of a speaker, --edge-absorption (id,t|b|l|r,value), eg --edge-absorption (1,t,0.5)");
        }

        void ChangeEdgeAbsorptionEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("edge-absorption") == 0)
                return;

            std::vector<CLIEdgeAbsorption> edges = input["edge-absorption"].as<std::vector<CLIEdgeAbsorption>>();

            for (int i = 0; i < edges.size(); ++i)
            {
                if (edges[i].edge == 't')
                    model->Domains[edges[i].id].T.Absorption = edges[i].value;
                else if (edges[i].edge == 'b')
                    model->Domains[edges[i].id].B.Absorption = edges[i].value;
                else if (edges[i].edge == 'l')
                    model->Domains[edges[i].id].L.Absorption = edges[i].value;
                else if (edges[i].edge == 'r')
                    model->Domains[edges[i].id].R.Absorption = edges[i].value;
            }
        }

        void ChangeEdgeLREditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("edge-local-reacting,l",
                       value<std::vector<CLIEdgeLR>>(),
                       "change the LR of a speaker, --edge-local-reacting (id,t|b|l|r,value), eg --edge-local-reacting (1,t,true)");
        }

        void ChangeEdgeLREditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("edge-local-reacting") == 0)
                return;

            std::vector<CLIEdgeLR> edges = input["edge-local-reacting"].as<std::vector<CLIEdgeLR>>();

            for (int i = 0; i < edges.size(); ++i)
            {
                if (edges[i].edge == 't')
                    model->Domains[edges[i].id].T.LR = edges[i].value;
                else if (edges[i].edge == 'b')
                    model->Domains[edges[i].id].B.LR = edges[i].value;
                else if (edges[i].edge == 'l')
                    model->Domains[edges[i].id].L.LR = edges[i].value;
                else if (edges[i].edge == 'r')
                    model->Domains[edges[i].id].R.LR = edges[i].value;
            }
        }

        void AddSpeakerEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("speaker-add,p",
                       value<std::vector<CLISpeakerReceiverAdd>>(),
                       "Add a speaker, --speaker-add [x,y], eg --speaker-add [0,0]");
        }

        void AddSpeakerEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("speaker-add") == 0)
                return;

            std::vector<CLISpeakerReceiverAdd> add = input["speaker-add"].as<std::vector<CLISpeakerReceiverAdd>>();

            for (int i = 0; i < add.size(); ++i)
            {
                model->Speakers.push_back(QVector3D(add[i].x, add[i].y, 0));
            }
        }

        void RemoveSpeakerEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("speaker-remove,P",
                       value<int>(&delIndex)->default_value(-1),
                       "Remove a speaker, --speaker-remove id");
        }

        void RemoveSpeakerEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (delIndex == -1)
                return;

            model->Speakers.erase(model->Speakers.begin() + delIndex);
        }

        void AddReceiverEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("receiver-add,r",
                       value<std::vector<CLISpeakerReceiverAdd>>(),
                       "Add a receiver, --receiver-add [x,y], eg --receiver-add [0,0]");
        }

        void AddReceiverEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("receiver-add") == 0)
                return;

            std::vector<CLISpeakerReceiverAdd> add = input["receiver-add"].as<std::vector<CLISpeakerReceiverAdd>>();

            for (int i = 0; i < add.size(); ++i)
            {
                model->Receivers.push_back(QVector3D(add[i].x, add[i].y, 0));
            }
        }

        void RemoveReceiverEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("receiver-remove,R",
                       value<int>(&delIndex)->default_value(-1),
                       "Remove a receiver, --receiver-remove id");
        }

        void RemoveReceiverEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (delIndex == -1)
                return;

            model->Receivers.erase(model->Speakers.begin() + delIndex);
        }

        void SetOptionEditCommandPart::AddOptions(options_description_easy_init add_option)
        {
            add_option("grid-spacing", value<float>(), "help")
                    ("patch-error", value<float>(), "help")
                    ("window-size", value<float>(), "help")
                    ("render-time", value<float>(), "help")
                    ("pml-cells", value<int>(), "help")
                    ("attenuation-of-pml-cells", value<float>(), "help")
                    ("density-of-air", value<float>(), "help")
                    ("max-frequency", value<float>(), "help")
                    ("sound-speed", value<float>(), "help")
                    ("fact-rk", value<float>(), "help")
                    ("save-nth", value<int>(), "help")
                    ("bandwidth", value<float>(), "help")
                    ("spectral-interpolation", value<float>(), "help")
                    ("wave-length", value<float>(), "help")
                    ("time-step", value<float>(), "help")
                    ("rk-coefficients", value<std::vector<float>>()->multitoken(), "help")
                //todo fix these arguments
                //("window", value<Eigen::ArrayXf>(), "help")
                    ;
        }

        void SetOptionEditCommandPart::Execute(std::shared_ptr<Kernel::PSTDConfiguration> model, variables_map input)
        {
            if (input.count("grid-spacing") > 0) model->Settings.SetGridSpacing(input["grid-spacing"].as<float>());
            if (input.count("patch-error") > 0) model->Settings.SetPatchError(input["patch-error"].as<float>());
            //todo fix this option
            //if(input.count("window-size") > 0) model->Settings.SetWindowSize(input["window-size"].as<float>());
            if (input.count("render-time") > 0) model->Settings.SetRenderTime(input["render-time"].as<float>());
            if (input.count("pml-cells") > 0) model->Settings.SetPMLCells(input["pml-cells"].as<int>());
            if (input.count("attenuation-of-pml-cells") > 0)
                model->Settings.SetAttenuationOfPMLCells(input["attenuation-of-pml-cells"].as<float>());
            if (input.count("density-of-air") > 0) model->Settings.SetDensityOfAir(input["density-of-air"].as<float>());
            if (input.count("max-frequency") > 0) model->Settings.SetMaxFrequency(input["max-frequency"].as<float>());
            if (input.count("sound-speed") > 0) model->Settings.SetSoundSpeed(input["sound-speed"].as<float>());
            if (input.count("fact-rk") > 0) model->Settings.SetFactRK(input["fact-rk"].as<float>());
            if (input.count("save-nth") > 0) model->Settings.SetSaveNth(input["save-nth"].as<int>());
            if (input.count("bandwidth") > 0) model->Settings.SetBandWidth(input["bandwidth"].as<float>());
            if (input.count("spectral-interpolation") > 0)
                model->Settings.SetSpectralInterpolation(input["spectral-interpolation"].as<bool>());
            if (input.count("wave-length") > 0) model->Settings.SetWaveLength(input["wave-length"].as<float>());
            //todo fix this option
            //if(input.count("time-step") > 0) model->Settings.SetTimeStep(input["time-step"].as<float>());
            if (input.count("rk-coefficients") > 0)
                model->Settings.SetRKCoefficients(input["rk-coefficients"].as<std::vector<float>>());
            //if(input.count("window") > 0) model->Settings.SetWindow(input["window"].as<Eigen::ArrayXf>());
        }
    }
}