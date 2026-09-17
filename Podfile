platform :osx, '12.0'

target 'Vimac' do
  use_frameworks!

  pod 'AXSwift', '~> 0.2'
  pod 'RxSwift', '~> 5'
  pod 'RxCocoa', '~> 5'
  pod 'MASShortcut'
  # Removed Sparkle
  pod 'Preferences'

  target 'VimacTests' do
    inherit! :search_paths
  end

  target 'VimacUITests' do
    inherit! :search_paths
  end
end

post_install do |installer|
  installer.pods_project.targets.each do |target|
    target.build_configurations.each do |config|
      config.build_settings['MACOSX_DEPLOYMENT_TARGET'] = '12.0'
    end
  end
end
