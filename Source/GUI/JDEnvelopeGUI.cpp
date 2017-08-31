#include "JDEnvelopeGUI.hpp"

JDEnvelopeGUI::JDEnvelopeGUI()
{
    m_constrainer = new ComponentBoundsConstrainer();
    
    std::unique_ptr<Slider> firstSlider (makeNewCurveSlider());
    addAndMakeVisible(firstSlider.get());
    firstSlider->addListener(this);
    m_curveSliders.push_back(std::move(firstSlider));
}

void JDEnvelopeGUI::paint(juce::Graphics &g)
{
    
    Colour bgCol (Colours::slategrey.withAlpha((float)0.9));
    g.fillAll(bgCol);
    g.setColour(Colours::black);
    g.drawRect(m_envBounds);
    Path p;
    if (m_nodeHandles.size() > 0)
    {
        Point<float> previousPoint (m_envBounds.getX(), m_envBounds.getBottom());
        p.startNewSubPath(previousPoint);
        for (auto &handle : m_nodeHandles)
        {
            auto currentPoint = handle->getBounds().getCentre().toFloat();
            drawExpLine(p, Line<float> (previousPoint, currentPoint), m_curveSliders[handle->index()]->getValue(), 1);
            previousPoint = currentPoint;
        }
        drawExpLine(p, Line<float> (previousPoint, Point<float> (m_envBounds.getWidth(), m_envBounds.getHeight())), m_curveSliders.back()->getValue(), 1);
        g.strokePath(p, PathStrokeType(2));
    }
}

void JDEnvelopeGUI::resized() {
    auto r = getLocalBounds();
    r.removeFromBottom(m_handleSize);
    m_envBounds = r;
}

void JDEnvelopeGUI::getNewEnvelope(jd::Envelope<float> &env, float length)
{
    env.clear();
    env.addFirstNode(0.);
    int i = 0;
    float timeSum = 0.;
    for (auto &nh : m_nodeHandles)
    {
        auto normalisedPos = nh->getCentre().toFloat() / Point<float> ((float)m_envBounds.getWidth(), (float)m_envBounds.getHeight());
        float level = 1. - normalisedPos.getY();
        float time = normalisedPos.getX() - timeSum;
        float curve = m_curveSliders[i]->getValue();
        env.addNode(level, time * length, curve);
        timeSum += time;
        i++;
    }
    env.addNode(0., (1. - timeSum) * length, 1.);
}

void JDEnvelopeGUI::setShouldAddHandleOnDoubleClick(bool shouldAddHandleOnDoubleClick)
{
    m_shouldAddHandleOnDoubleClick = shouldAddHandleOnDoubleClick;
}

void JDEnvelopeGUI::sliderValueChanged(juce::Slider *slider)
{
    repaint();
}

void JDEnvelopeGUI::mouseDown(const juce::MouseEvent &event)
{
    if (event.getNumberOfClicks() == 2) {
        if (m_shouldAddHandleOnDoubleClick)
            addHandle(event.getPosition());
    }
}

void JDEnvelopeGUI::makeFromEnvelope(const jd::Envelope<float>& env)
{
    removeAllHandles();
    const float fullDuration = std::accumulate(env.times.begin(), env.times.end(), 0.);
    const int numNodes = env.times.size();
    float time = 0.;
    for (int i = 0; i < numNodes - 1; i++){
        time += env.times[i];
        addHandle({ static_cast<int>((time / fullDuration) * (float)m_envBounds.getWidth()),
            static_cast<int>((1.f - (float)env.levels[i + 1]) * (float)m_envBounds.getHeight()) });
    };
}

Slider* JDEnvelopeGUI::makeNewCurveSlider()
{
    Slider* newSlider = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::NoTextBox);
    
    newSlider->setRange(0.1, 10.);
    newSlider->setSkewFactorFromMidPoint(1.);
    newSlider->setValue(1.);
    return newSlider;
}

void JDEnvelopeGUI::removeHandle(const int indexToRemoveAt)
{
    removeChildComponent(m_nodeHandles[indexToRemoveAt].get());
    removeChildComponent(m_curveSliders[indexToRemoveAt].get());
    
    m_nodeHandles.erase(m_nodeHandles.begin() + indexToRemoveAt);
    m_curveSliders.erase(m_curveSliders.begin() + indexToRemoveAt);
    
    int indexToBeginUpdating = indexToRemoveAt;
    for (auto i = (m_nodeHandles.begin()) + indexToBeginUpdating; i != m_nodeHandles.end(); i++)
        (*i)->setIndex(indexToBeginUpdating++);
    
    updateSliderBounds();
    repaint();
}

void JDEnvelopeGUI::removeAllHandles()
{
    for (auto ri = m_nodeHandles.rbegin(); ri != m_nodeHandles.rend(); ri++)
        removeHandle((*ri)->index());
}

void JDEnvelopeGUI::swapNodeHandles(int first, int second) {
    std::swap(m_nodeHandles[first], m_nodeHandles[second]);
    int index = 0;
    for (auto i = (m_nodeHandles.begin()); i != m_nodeHandles.end(); i++)
        i[0]->setIndex(index++);
}

void JDEnvelopeGUI::updateSliderBounds() {
    auto r = getLocalBounds().removeFromBottom(10);
    int amountAlreadyRemovedFromLeft = 0;
    for (int i = 0; i < m_nodeHandles.size(); i++)
    {
        int amountToRemoveFromLeft = m_nodeHandles[i]->getCentreX() - amountAlreadyRemovedFromLeft;
        m_curveSliders[i]->setBounds(r.removeFromLeft(amountToRemoveFromLeft));
        amountAlreadyRemovedFromLeft += amountToRemoveFromLeft;
    }
    m_curveSliders[m_nodeHandles.size()]->setBounds(r);
}
