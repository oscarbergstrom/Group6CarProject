/**
 * cockpit - Visualization environment
 * Copyright (C) 2012 - 2015 Christian Berger
 * Copyright (C) 2008 - 2011 (as monitor component) Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLUGINS_ENVIRONMENTVIEWER_ENVIRONMENTVIEWERWIDGET_H_
#define PLUGINS_ENVIRONMENTVIEWER_ENVIRONMENTVIEWERWIDGET_H_

#include <QtCore>
#include <QtGui>

#include <map>
#include <string>
#include <vector>

#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/io/conference/ContainerListener.h"
#include "opendavinci/odcore/strings/StringComparator.h"
#include "opendlv/threeD/NodeDescriptor.h"
#include "plugins/environmentviewer/CameraAssignableNodesListener.h"
#include "plugins/environmentviewer/SelectableNodeDescriptorTreeListener.h"

class QComboBox;
class QTreeWidget;
class QTreeWidgetItem;
namespace cockpit { namespace plugins { class GLControlFrame; } }
namespace cockpit { namespace plugins { class PlugIn; } }
namespace odcore { namespace base { template <typename T> class TreeNode; } }
namespace odcore { namespace data { class Container; } }

namespace cockpit {
    namespace plugins {
        namespace environmentviewer {

            /**
             * This class is the outer container for the 3D scene graph viewer
             * implemented in EnvironmentViewerGLWidget and a tree like data
             * structure on its right hand side.
             */
class EnvironmentViewerGLWidget;
class SelectableNodeDescriptor;

            class EnvironmentViewerWidget : public QWidget, public odcore::io::conference::ContainerListener, public CameraAssignableNodesListener, public SelectableNodeDescriptorTreeListener {

                Q_OBJECT

                private:
                    /**
                     * "Forbidden" copy constructor. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the copy constructor.
                     */
                    EnvironmentViewerWidget(const EnvironmentViewerWidget &/*obj*/);

                    /**
                     * "Forbidden" assignment operator. Goal: The compiler should warn
                     * already at compile time for unwanted bugs caused by any misuse
                     * of the assignment operator.
                     */
                    EnvironmentViewerWidget& operator=(const EnvironmentViewerWidget &/*obj*/);

                public:
                    /**
                     * Constructor.
                     *
                     * @param plugIn Reference to the plugin to which this widget belongs.
                     * @param prnt Pointer to the parental widget.
                     */
                    EnvironmentViewerWidget(const PlugIn &plugIn, QWidget *prnt);

                    virtual ~EnvironmentViewerWidget();

                    virtual void nextContainer(odcore::data::Container &c);

                    virtual void update(odcore::base::TreeNode<SelectableNodeDescriptor> *node);

                    virtual void updateListOfCameraAssignableNodes(const vector<opendlv::threeD::NodeDescriptor> &list);

                private slots:
                    /**
                     * This method is called whenever an item in the list changes
                     * its state.
                     *
                     * @param item Item that changed.
                     * @param column Number of column.
                     */
                    void itemChanged(QTreeWidgetItem *item, int column);

                    /**
                     * This method is called whenever an item in the combobox
                     * for the assignable camera changes its state.
                     *
                     * @param item Item that changed.
                     */
                    void selectedItemChanged(const QString &item);

                    /**
                     * This method is called whenever an item in the combobox
                     * for the color of the background changes its state.
                     *
                     * @param item Item that changed.
                     */
                    void selectedItemChangedBackground(const QString &item);

                private:
                    GLControlFrame *m_viewerControlWidget;
                    EnvironmentViewerGLWidget *m_environmentViewerGLWidget;

                    QComboBox *m_cameraSelector;
                    odcore::base::Mutex m_cameraAssignableNodesUpdateMutex;
                    bool m_cameraAssignableNodesUpdate;
                    map<string, opendlv::threeD::NodeDescriptor, odcore::strings::StringComparator> m_mapOfNodeDescriptors;

                    QComboBox *m_backgroundSelector;

                    QTreeWidget *m_textualSceneGraph;
                    odcore::base::Mutex m_textualSceneGraphRootUpdateMutex;
                    bool m_textualSceneGraphRootUpdate;
                    odcore::base::Mutex m_selectableNodeDescriptorTreeMutex;
                    odcore::base::TreeNode<SelectableNodeDescriptor> *m_selectableNodeDescriptorTree;

                    /**
                     * This method updates the tree of SelectableNodeDescriptors.
                     *
                     * @param name name of the NodeDescriptor to be updated.
                     * @param enabled True if the NodeDescriptor is enabled.
                     */
                    void updateEntry(odcore::base::TreeNode<SelectableNodeDescriptor> *node, const string &name, const bool &enabled);
            };
        }
    }
} // plugins::environmentviewer

#endif /*PLUGINS_ENVIRONMENTVIEWER_ENVIRONMENTVIEWERWIDGET_H_*/
