/* Copyright 2013 Little IO
 *
 * termite is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * termite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with termite.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef TERMITE_SYNTH_H
#define TERMITE_SYNTH_H

#include "termite_common.h"
#include "voice_handler.h"

#include <vector>

#define MOD_MATRIX_SIZE 6

namespace laf {
  class Add;
  class Envelope;
  class Filter;
  class Interpolate;
  class LinearSlope;
  class Multiply;
  class Oscillator;
  class SmoothValue;

  class TermiteVoiceHandler : public VoiceHandler {
    public:
      TermiteVoiceHandler();

      control_map getControls() { return controls_; }

      void setModWheel(laf_float value);
      void setPitchWheel(laf_float value);
      void setModulationSource(int index, std::string source);
      void setModulationDestination(int index, std::string destination);

    private:
      void createArticulation(Output* note, Output* velocity, Output* trigger);
      void createOscillators(Output* frequency, Output* reset);
      void createFilter(Output* audio, Output* keytrack, Output* reset);
      void createModMatrix();

      Add* note_from_center_;
      SmoothValue* mod_wheel_amount_;
      SmoothValue* pitch_wheel_amount_;
      LinearSlope* current_frequency_;
      Envelope* amplitude_envelope_;
      Multiply* amplitude_;

      Oscillator* oscillator1_;
      Oscillator* oscillator2_;
      Oscillator* lfo1_;
      Oscillator* lfo2_;
      Interpolate* oscillator_mix_;

      Filter* filter_;
      Envelope* filter_envelope_;

      Multiply* output_;

      control_map controls_;
      output_map mod_sources_;
      input_map mod_destinations_;

      std::vector<std::string> mod_source_names_;
      std::vector<std::string> mod_destination_names_;
      Value* mod_matrix_scales_[MOD_MATRIX_SIZE];
      Multiply* mod_matrix_[MOD_MATRIX_SIZE];
      std::string current_mod_destinations_[MOD_MATRIX_SIZE];
  };

  class MatrixSourceValue : public Value {
    public:
      MatrixSourceValue(TermiteVoiceHandler* handler) :
          Value(0), handler_(handler), mod_index_(0) { }

      virtual Processor* clone() const { return new MatrixSourceValue(*this); }

      void setSources(const std::vector<std::string> &sources) {
        sources_ = sources;
      }

      void setModulationIndex(int mod_index) {
        mod_index_ = mod_index;
      }

      void set(laf_float value) {
        Value::set(static_cast<int>(value));
        handler_->setModulationSource(mod_index_, sources_[value_]);
      }

    private:
      TermiteVoiceHandler* handler_;
      std::vector<std::string> sources_;
      int mod_index_;
  };

  class MatrixDestinationValue : public Value {
    public:
      MatrixDestinationValue(TermiteVoiceHandler* handler) :
          Value(0), handler_(handler), mod_index_(0) { }

      virtual Processor* clone() const {
        return new MatrixDestinationValue(*this);
      }

      void setDestinations(const std::vector<std::string> &destinations) {
        destinations_ = destinations;
      }

      void setModulationIndex(int mod_index) {
        mod_index_ = mod_index;
      }

      void set(laf_float value) {
        Value::set(static_cast<int>(value));
        handler_->setModulationDestination(mod_index_, destinations_[value_]);
      }

    private:
      TermiteVoiceHandler* handler_;
      std::vector<std::string> destinations_;
      int mod_index_;
  };

  class TermiteSynth : public ProcessorRouter {
    public:
      TermiteSynth();

      control_map getControls();

      void setModWheel(laf_float value) {
        voice_handler_->setModWheel(value);
      }
      void setPitchWheel(laf_float value) {
        voice_handler_->setPitchWheel(value);
      }
      void noteOn(laf_float note, laf_float velocity = 1.0);
      void noteOff(laf_float note);
      void sustainOn() { voice_handler_->sustainOn(); }
      void sustainOff() { voice_handler_->sustainOff(); }

    private:
      TermiteVoiceHandler* voice_handler_;

      control_map controls_;
  };
} // namespace laf

#endif // TERMITE_SYNTH_H
