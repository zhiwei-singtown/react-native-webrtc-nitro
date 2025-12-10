import { type HybridObject } from 'react-native-nitro-modules'
import { NitroModules } from 'react-native-nitro-modules'

export type PermissionState = 'denied' | 'granted' | 'prompt'
type PermissionName = 'camera' | 'microphone'

export interface PermissionDescriptor {
  name: PermissionName
}

interface Permissions extends HybridObject<{
  ios: 'swift'
  android: 'kotlin'
}> {
  query(permissionDesc: PermissionDescriptor): Promise<PermissionState>
  request(permissionDesc: PermissionDescriptor): Promise<PermissionState>
}

const PermissionsExport =
  NitroModules.createHybridObject<Permissions>('Permissions')
type PermissionsExport = Permissions
export { PermissionsExport as Permissions }
