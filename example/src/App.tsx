import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Home from './Home';
import Camera from './Camera';
import Offer from './Offer';
import Answer from './Answer';
import SelfVideo from './SelfVideo';

const Stack = createNativeStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Home" component={Home} />
        <Stack.Screen name="Camera" component={Camera} />
        <Stack.Screen name="Offer" component={Offer} />
        <Stack.Screen name="Answer" component={Answer} />
        <Stack.Screen name="SelfVideo" component={SelfVideo} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
