#ifndef JDEnvelopeGUI_hpp
#define JDEnvelopeGUI_hpp

#include <../JuceLibraryCode/JuceHeader.h>
#include "jdHeader.h"
#include <numeric>
/*
    Currently updates everything on every change - not very efficient 
    improvement would be to allocate array of slider/nodeHandle ptrs
    + envelope with maximum size and update individual elements (what about node handle indices???)
 */
class JDEnvelopeGUI : public Component,
public Slider::Listener
{
public:
    JDEnvelopeGUI();

    void paint (Graphics &g) override;
    void resized() override;

    void getNewEnvelope (jd::Envelope<float>& env, float length);
    
    void setNodeHandlesVisible(bool shouldBeVisible)
    {
        for (auto &nh:m_nodeHandles)
        {
            nh->setVisible(shouldBeVisible);
            m_curveSliders[nh->index()]->setVisible(shouldBeVisible);
        }
        m_curveSliders.back()->setVisible(shouldBeVisible);
    }
    void setShouldAddHandleOnDoubleClick(bool shouldAddHandleOnDoubleClick);
    
    void sliderValueChanged (Slider* slider) override;
    
    void mouseDown(const MouseEvent &event) override;
    
    void makeFromEnvelope(const jd::Envelope<float>& env);
    
//private:
    class NodeHandle :
    public Component,
    public ComponentDragger
    {
    public:
        NodeHandle(){}

        void paint (Graphics &g) override
        {
            if (m_isSelected) {
                g.setColour(Colours::darkorange);
            }
            else {
                g.setColour(Colours::darkgrey);
            }
            
            g.fillEllipse(Rectangle<float> (getLocalBounds().toFloat()).reduced(5));
        };

        void mouseDown  (const MouseEvent &event) override {
            if (event.getNumberOfClicks() == 1)
            {
                startDraggingComponent(this, event);
                m_isSelected = true;
                repaint();
            }
        }
        void mouseUp    (const MouseEvent &event) override {
            m_isSelected = false;
            constrainPosition();
            if (event.mods.isAltDown())
                ((JDEnvelopeGUI*)getParentComponent())->removeHandle(index());
            repaint();
        }
        void mouseDrag  (const MouseEvent &event) override {
            JDEnvelopeGUI* parent = (JDEnvelopeGUI*)getParentComponent();
            dragComponent(this, event, nullptr);
            constrainPosition();
            
            if (index() < parent->m_nodeHandles.size()-1)
                if(getCentreX() > parent->m_nodeHandles[index() + 1]->getCentreX())
                    parent->swapNodeHandles(index(), index() + 1);
            
            if (index() > 0)
                if(getCentreX() < parent->m_nodeHandles[index() - 1]->getCentreX())
                    parent->swapNodeHandles(index(), index() - 1);
            
            constrainPosition();
            parent->updateSliderBounds();
            parent->repaint();
        }

        void constrainPosition() {
            auto parent = (JDEnvelopeGUI*)getParentComponent();
            const auto bounds = parent->m_envBounds;
            if (getCentreX()  <  bounds.getX()) setCentrePosition(bounds.getX(), getCentreY());
            if (getCentreX()  >  bounds.getRight()) setCentrePosition(bounds.getRight(), getCentreY());
            if (getCentreY()  <  bounds.getY()) setCentrePosition(getCentreX(), bounds.getY());
            if (getCentreY()  >  bounds.getBottom()) setCentrePosition(getCentreX(), bounds.getBottom());
        }
        Point<int>  getCentre() { return getPosition() + Point<int> (getWidth(), getHeight()) * 0.5; };
        int   getCentreX() { return getPosition().getX() + getWidth() * 0.5; };
        int   getCentreY() { return getPosition().getY() + getHeight() * 0.5; };
        void  setIndex (const int index) { m_index = index; }
        int   index() { return m_index; }
    private:
        bool m_isSelected = false;
        int m_index;
    };

    void addHandle      (Point<int> positionToAddAt) {
        std::unique_ptr<NodeHandle> newHandle(new NodeHandle());
        addAndMakeVisible(newHandle.get());
        newHandle->setBounds(
                             positionToAddAt.getX() - m_halfHandleSize,
                             positionToAddAt.getY() - m_halfHandleSize,
                             m_handleSize,
                             m_handleSize);
        int indexToInsertHandle = 0;
        for (auto  &handle : m_nodeHandles)
        {
            if (newHandle->getBounds().getCentreX() < handle->getBounds().getCentreX())
                break;
            else
                indexToInsertHandle++;
        }
        newHandle->setIndex(indexToInsertHandle);
        m_nodeHandles.insert(m_nodeHandles.begin() + indexToInsertHandle, std::move(newHandle));
        int indexToBeginUpdating = indexToInsertHandle;
        for (auto i = (m_nodeHandles.begin()) + indexToBeginUpdating; i != m_nodeHandles.end(); i++)
            (*i)->setIndex(indexToBeginUpdating++);

        //Slider
        std::unique_ptr<Slider> newSlider (makeNewCurveSlider());
        addAndMakeVisible(newSlider.get());
        newSlider->addListener(this);
        m_curveSliders.insert(m_curveSliders.begin() + indexToInsertHandle, std::move(newSlider));
        updateSliderBounds();
        
        repaint();
    }
    void drawExpLine    (Path &path, const Line<float> line, const float curve, int step = 10)
    {
        if (curve != 0) {
            path.lineTo(line.getStart());
            const float w = line.getEndX() - line.getStartX();
            const float h = line.getEndY() - line.getStartY();
            for (int px = 0; px < w; px+=step)
            {
                Point<float> pnt (
                                  line.getStartX() + px,
                                  line.getStartY() + ::powf((px/w), curve) * h
                                  );
                path.lineTo(pnt);
            }
            path.lineTo(line.getEnd());
        }
    }
    
    Slider* makeNewCurveSlider();
    void removeHandle(const int indexToRemoveAt);
    void removeAllHandles();
    void swapNodeHandles(int first, int second);
    void updateSliderBounds();
    
    Rectangle<int>  m_envBounds;
    ScopedPointer<ComponentBoundsConstrainer> m_constrainer;
    std::vector< std::unique_ptr<NodeHandle>> m_nodeHandles;
    std::vector< std::unique_ptr<Slider>> m_curveSliders;
    const int m_handleSize { 20 };
    const int m_halfHandleSize { static_cast<int>(m_handleSize * 0.5) };
    bool m_shouldAddHandleOnDoubleClick {true};
};


#endif /* JDEnvelopeGUI_hpp */
