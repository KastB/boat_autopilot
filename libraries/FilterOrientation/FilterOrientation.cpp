#include "FilterOrientation.h"
#include <math.h>

FilterOrientation::FilterOrientation(FILTER_TYPE ft, float fc,
                                     float initialValue) {
  m_lowpassFilter = new FilterOnePole(ft, fc, initialValue);
  m_lowpassFilter->setToNewValue(-1.0f);
}

FilterOrientation::~FilterOrientation() { delete (m_lowpassFilter); }

void FilterOrientation::setToNewValue(float val) {
  m_lowpassFilter->setToNewValue(val);
}

float FilterOrientation::input(float y) {
  // initialization
  if (m_lowpassFilter->output() < 0.0f ||
      isnan(m_lowpassFilter->output())) {
    m_lowpassFilter->setToNewValue(y);
    return y;
  }

  while (m_lowpassFilter->output() < y - 180.0f) {
    y -= 360.0f;
  }
  while (m_lowpassFilter->output() > y + 180.0f) {
    y += 360.0f;
  }

  m_lowpassFilter->input(y);

  while (m_lowpassFilter->output() >= 360.0f) {
    m_lowpassFilter->setToNewValue(m_lowpassFilter->output() - 360.0f);
  }
  while (m_lowpassFilter->output() < 0.0f) {
    m_lowpassFilter->setToNewValue(m_lowpassFilter->output() + 360.0f);
  }
  return m_lowpassFilter->output();
}

void FilterOrientation::setFrequency(float newFrequency) {
  m_lowpassFilter->setFrequency(newFrequency);
}

float FilterOrientation::output() { return m_lowpassFilter->output(); }
